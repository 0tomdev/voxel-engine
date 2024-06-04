#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ChunkMesh.hpp"
#include "utils.hpp"
#include "block/Block.hpp"

// https://github.com/jdah/minecraft-again/blob/master/src/level/chunk_renderer.cpp 😋
// Right handed system: https://learnopengl.com/Getting-started/Coordinate-Systems
/*  3D CUBE
 *  0-------1
 *  | 2-----+-3
 *  | |     | |
 *  | |     | |
 *  4-+-----5 |
 *    6-------7
 *
 * TOP (y=1)
 * 0-------1
 * |       |
 * |       |
 * |       |
 * 2-------3
 *
 * BOTTOM (y=0)
 * 4-------5
 * |       |
 * |       |
 * |       |
 * 6-------7
 *
 * east (+x) -->
 * west (-x) <--
 * up (+y) ⬆
 * down (-y) ⬇
 * south (+z) into your face
 * north (-z) into the screen
 *
 *
 */

// clang-format off
static std::vector<ChunkMesh::Vertex> allCubeVertices = {
    // +x
    { 1, 0, 0, 1, 0, 0 }, { 1, 0, 1, 0, 0, 0 }, { 1, 1, 0, 1, 1, 0 }, { 1, 1, 1, 0, 1, 0 },

    // -x
    { 0, 0, 0, 0, 0, 1 }, { 0, 1, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 1 }, { 0, 1, 1, 1, 1, 1 },

    // +y
    { 0, 1, 0, 0, 1, 2 }, { 1, 1, 0, 1, 1, 2 }, { 0, 1, 1, 0, 0, 2 }, { 1, 1, 1, 1, 0, 2 },

    // -y
    { 0, 0, 0, 0, 0, 3 }, { 0, 0, 1, 0, 1, 3 }, { 1, 0, 0, 1, 0, 3 }, { 1, 0, 1, 1, 1, 3 },

    // +z
    { 0, 0, 1, 0, 0, 4 }, { 0, 1, 1, 0, 1, 4 }, { 1, 0, 1, 1, 0, 4 }, { 1, 1, 1, 1, 1, 4 },

    // -z
    { 0, 1, 0, 0, 1, 5 }, { 0, 0, 0, 0, 0, 5 }, { 1, 1, 0, 1, 1, 5 }, { 1, 0, 0, 1, 0, 5 },
};
// clang-format on

static std::optional<Shader> shader;
static unsigned int vertexColorLocation = -1;
static unsigned int modelLoc = -1;
static unsigned int viewLoc = -1;
static unsigned int projectionLoc = -1;
static unsigned int opacityLoc = -1;

/**
 * Load shaders here because GL functions can only be called after glewInit()
 * */
void ChunkMesh::init() {
    shader = Shader("./assets/shaders/vertex.glsl", "./assets/shaders/frag.glsl");
    const Shader& shaderValue = shader.value();
    vertexColorLocation = glGetUniformLocation(shaderValue.ID, "time");
    modelLoc = glGetUniformLocation(shaderValue.ID, "model");
    viewLoc = glGetUniformLocation(shaderValue.ID, "view");
    projectionLoc = glGetUniformLocation(shaderValue.ID, "projection");
    opacityLoc = glGetUniformLocation(shaderValue.ID, "opacity");
}

void ChunkMesh::createMeshBetter(const Chunk& chunk) {
    assert(vertexSize == sizeof(Vertex));
    utils::ScopeTimer timer;

    using Direction = utils::Direction;

    for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
                auto pos = glm::vec3(x, y, z);
                BlockId block = chunk.getBlock(pos);

                BlockId prevXBlock = 0;
                if (x > 0) prevXBlock = chunk.getBlock({x - 1, y, z});
                BlockId prevYBlock = 0;
                if (y > 0) prevYBlock = chunk.getBlock({x, y - 1, z});
                BlockId prevZBlock = 0;
                if (z > 0) prevZBlock = chunk.getBlock({x, y, z - 1});

                // Faces only get added to the current block if they are facing west (1),
                // down (3), or north (5)

                if (block && !prevXBlock) {
                    addFace(pos, Direction::WEST, chunk);
                } else if (!block && prevXBlock) {
                    addFace(pos - glm::vec3(1, 0, 0), Direction::EAST, chunk);
                }

                if (block && !prevYBlock) {
                    addFace(pos, Direction::DOWN, chunk);
                } else if (!block && prevYBlock) {
                    addFace(pos - glm::vec3(0, 1, 0), Direction::UP, chunk);
                }

                if (block && !prevZBlock) {
                    addFace(pos, Direction::NORTH, chunk);
                } else if (!block && prevZBlock) {
                    addFace(pos - glm::vec3(0, 0, 1), Direction::SOUTH, chunk);
                }

                // Faces on east, up, and south edge of chunk
                if (x == Chunk::CHUNK_SIZE - 1 && block) {
                    addFace(pos, Direction::EAST, chunk);
                }
                if (y == Chunk::CHUNK_HEIGHT - 1 && block) {
                    addFace(pos, Direction::UP, chunk);
                }
                if (z == Chunk::CHUNK_SIZE - 1 && block) {
                    addFace(pos, Direction::SOUTH, chunk);
                }
            }
        }
    }

    // std::cout << "Size of chunk mesh: " << triangleVerts.size() * vertexSize / 1024
    //           << " kb\n";

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, triangleVerts.size() * vertexSize, &triangleVerts[0], GL_STATIC_DRAW
    );

    // xyz coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glEnableVertexAttribArray(0);
    // uv coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, u)));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2, 1, GL_UNSIGNED_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        3, 1, GL_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, textureIdx))
    );
    glEnableVertexAttribArray(3);

    // for (auto& v : triangleVerts) {
    //     std::cout << v.pos.x << ", " << v.pos.y << ", " << v.pos.z << "\n";
    // }
}

void ChunkMesh::addFace(const glm::vec3& pos, utils::Direction facing, const Chunk& chunk) {
    addQuad(pos, facing, Block::blockDefs[chunk.getBlock(pos)].getTextureIdx(facing));
}

void ChunkMesh::addQuad(const glm::vec3& pos, int facing, int textureIdx) {
    Vertex v1 = allCubeVertices[facing * 4 + 0];
    Vertex v2 = allCubeVertices[facing * 4 + 1];
    Vertex v3 = allCubeVertices[facing * 4 + 2];
    Vertex v4 = allCubeVertices[facing * 4 + 3];
    v1.pos += pos;
    v2.pos += pos;
    v3.pos += pos;
    v4.pos += pos;
    v1.textureIdx = textureIdx;
    v2.textureIdx = textureIdx;
    v3.textureIdx = textureIdx;
    v4.textureIdx = textureIdx;
    addTriangle(v1, v3, v2);
    addTriangle(v3, v4, v2);
}

void ChunkMesh::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    triangleVerts.push_back(v1);
    triangleVerts.push_back(v2);
    triangleVerts.push_back(v3);
}

ChunkMesh::ChunkMesh(const Chunk& chunk) {
    createMeshBetter(chunk);
}

void ChunkMesh::deleteBuffers() {
    std::cout << "Deleted mesh from VRAM\n";
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void ChunkMesh::render(const Camera& camera, float aspectRatio, glm::ivec2 worldIndex) const {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearClip, camera.farClip);
    glm::mat4 model = glm::mat4(1.0f);
    model =
        glm::translate(model, glm::vec3(worldIndex.x, 0, worldIndex.y) * (float)Chunk::CHUNK_SIZE);

    // Shader
    const Shader& shaderValue = shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)));
    GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniform1f(opacityLoc, 1.0f));

    // Render
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, triangleVerts.size()));
}

size_t ChunkMesh::getSize() const {
    return triangleVerts.size();
}
