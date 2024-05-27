#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ChunkMesh.hpp"
#include "utils.hpp"
#include "global.hpp"

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

static const size_t vertexSize = 5 * sizeof(float) + sizeof(unsigned int);

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

ChunkMesh::ChunkMesh(Chunk& chunk) {
    assert(vertexSize == sizeof(Vertex));
    utils::ScopeTimer timer;

    using Direction = utils::Direction;

    int i = 0;
    for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
                auto pos = glm::vec3(x, y, z);
                BlockID block = chunk.getBlock(pos);
                // std::cout << chunk.getIndex(pos) << " " << i << "\n";
                assert(block == chunk.data[i]);

                bool prevX = x > 0 ? chunk.getBlock({ x - 1, y, z }) : false;
                bool prevY = y > 0 ? chunk.getBlock({ x, y - 1, z }) : false;
                bool prevZ = z > 0 ? chunk.getBlock({ x, y, z - 1 }) : false;

                if (prevX != (bool)block) {
                    if (x == 0) {
                        addQuad(pos, Direction::WEST);
                    } else {
                        addQuad(pos - glm::vec3(1, 0, 0), Direction::EAST);
                    }
                }
                if (prevY != (bool)block) {
                    if (y == 0) {
                        addQuad(pos, Direction::DOWN);
                    } else {
                        addQuad(pos - glm::vec3(0, 1, 0), Direction::UP);
                    }
                }
                if (prevZ != (bool)block) {
                    if (z == 0) {
                        addQuad(pos, Direction::NORTH);
                    } else {
                        addQuad(pos - glm::vec3(0, 0, 1), Direction::SOUTH);
                    }
                }

                if (x == Chunk::CHUNK_SIZE - 1 && block) {
                    addQuad(pos, Direction::EAST);
                }
                if (y == Chunk::CHUNK_HEIGHT - 1 && block) {
                    addQuad(pos, Direction::UP);
                }
                if (z == Chunk::CHUNK_SIZE - 1 && block) {
                    addQuad(pos, Direction::SOUTH);
                }

                i++;
            }
        }
    }

    std::cout << "Size of chunk mesh: " << triangleVerts.size() * vertexSize / 1024
              << " kb\n";

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, triangleVerts.size() * vertexSize, &triangleVerts[0],
        GL_STATIC_DRAW
    );

    // xyz coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glEnableVertexAttribArray(0);
    // uv coords
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, u))
    );
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2, 1, GL_UNSIGNED_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);

    // for (auto& v : triangleVerts) {
    //     std::cout << v.pos.x << ", " << v.pos.y << ", " << v.pos.z << "\n";
    // }
}

void ChunkMesh::addQuad(const glm::vec3& pos, int facing) {
    Vertex v1 = allCubeVertices[facing * 4 + 0];
    Vertex v2 = allCubeVertices[facing * 4 + 1];
    Vertex v3 = allCubeVertices[facing * 4 + 2];
    Vertex v4 = allCubeVertices[facing * 4 + 3];
    v1.pos += pos;
    v2.pos += pos;
    v3.pos += pos;
    v4.pos += pos;
    addTriangle(v1, v3, v2);
    addTriangle(v3, v4, v2);
}

void ChunkMesh::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    triangleVerts.push_back(v1);
    triangleVerts.push_back(v2);
    triangleVerts.push_back(v3);
}

void ChunkMesh::render(Camera& camera) const {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
        100.0f
    );
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(1, 1, 1) * -0.01f);

    // Shader
    const Shader& shaderValue = Chunk::shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniformMatrix4fv(Chunk::viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(
        glUniformMatrix4fv(Chunk::projectionLoc, 1, GL_FALSE, glm::value_ptr(projection))
    );
    GL_CALL(glUniformMatrix4fv(Chunk::modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniform1f(Chunk::opacityLoc, 1.0f));

    // Render
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, triangleVerts.size()));
}