#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ChunkMesh.hpp"
#include "utils.hpp"
#include "block/Block.hpp"
#include "World.hpp"
#include <Instrumentor.h>

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
    shader = Shader("./assets/shaders/block.vert", "./assets/shaders/block.frag");
    const Shader& shaderValue = shader.value();
    vertexColorLocation = glGetUniformLocation(shaderValue.ID, "time");
    modelLoc = glGetUniformLocation(shaderValue.ID, "model");
    viewLoc = glGetUniformLocation(shaderValue.ID, "view");
    projectionLoc = glGetUniformLocation(shaderValue.ID, "projection");
    opacityLoc = glGetUniformLocation(shaderValue.ID, "opacity");
}

void ChunkMesh::createMesh(World& world) {
    assert(vertexSize == sizeof(Vertex));

    PROFILE_FUNCTION();

    auto startTime = std::chrono::high_resolution_clock::now();

    using Direction = utils::Direction;

    for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
                auto pos = glm::vec3(x, y, z);
                glm::ivec3 worldPos = Chunk::getWorldPosition(chunk.worldIndex, pos);
                BlockId block = nearChunks.getBlock(pos);

                BlockId prevXBlock = nearChunks.getBlock({x - 1, y, z});
                BlockId prevYBlock = 0;
                if (y > 0) prevYBlock = chunk.getBlock({x, y - 1, z});
                BlockId prevZBlock = nearChunks.getBlock({x, y, z - 1});

                // Faces only get added to the current block if they are facing west (1),
                // down (3), or north (5)

                if (shouldAddFace(block, prevXBlock)) {
                    addFace(pos, Direction::WEST);
                } else if (shouldAddFace(prevXBlock, block) && x > 0) {
                    addFace(pos - glm::vec3(1, 0, 0), Direction::EAST);
                }

                if (shouldAddFace(block, prevYBlock)) {
                    addFace(pos, Direction::DOWN);
                } else if (shouldAddFace(prevYBlock, block)) {
                    addFace(pos - glm::vec3(0, 1, 0), Direction::UP);
                }

                if (shouldAddFace(block, prevZBlock)) {
                    addFace(pos, Direction::NORTH);
                } else if (shouldAddFace(prevZBlock, block) && z > 0) {
                    addFace(pos - glm::vec3(0, 0, 1), Direction::SOUTH);
                }

                // Faces on east, up, and south edge of chunk
                if (x == Chunk::CHUNK_SIZE - 1 &&
                    shouldAddFace(block, nearChunks.getBlock({Chunk::CHUNK_SIZE, y, z}))) {
                    addFace(pos, Direction::EAST);
                }
                if (y == Chunk::CHUNK_HEIGHT - 1 && shouldAddFace(block, Block::AIR)) {
                    addFace(pos, Direction::UP);
                }
                if (z == Chunk::CHUNK_SIZE - 1 &&
                    shouldAddFace(block, nearChunks.getBlock({x, y, Chunk::CHUNK_SIZE}))) {
                    addFace(pos, Direction::SOUTH);
                }
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    generationTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

void ChunkMesh::addFace(const glm::vec3& pos, utils::Direction facing) {
    BlockId block = chunk.getBlock(pos);
    Mesh& mesh = Block::isTransparent(block) ? transparentMesh : opaqueMesh;
    addQuad(
        pos, facing, Block::blockDefs[block].getTextureIdx(facing), mesh,
        Block::blockDefs[block].isLiquid
    );
}

void ChunkMesh::addQuad(
    const glm::vec3& pos, int facing, int textureIdx, Mesh& mesh, bool isLiquid
) {
    Vertex v1 = allCubeVertices[facing * 4 + 0];
    Vertex v2 = allCubeVertices[facing * 4 + 1];
    Vertex v3 = allCubeVertices[facing * 4 + 2];
    Vertex v4 = allCubeVertices[facing * 4 + 3];

    std::vector<Vertex*> verts = {&v1, &v2, &v3, &v4};
    calculateAO(verts, (utils::Direction)facing, pos);

    if (isLiquid) {
        for (auto v : verts) {
            if (facing == utils::Direction::UP) {
                v->isLowered = 1;
            }
        }
    }

    v1.pos += pos;
    v2.pos += pos;
    v3.pos += pos;
    v4.pos += pos;
    v1.textureIdx = textureIdx;
    v2.textureIdx = textureIdx;
    v3.textureIdx = textureIdx;
    v4.textureIdx = textureIdx;

    addTriangle(v1, v3, v2, mesh);
    addTriangle(v3, v4, v2, mesh);
}

void ChunkMesh::addTriangle(Vertex v1, Vertex v2, Vertex v3, Mesh& mesh) {
    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);
}

void ChunkMesh::calculateAO(
    std::vector<Vertex*>& verts, utils::Direction facing, const glm::ivec3& pos
) const {
    using Direction = utils::Direction;

    for (auto v : verts) {
        int x, y, z;
        glm::ivec3 s1Pos, s2Pos, cPos;

        if (facing == Direction::UP || facing == Direction::DOWN) {
            x = v->pos.x == 0 ? -1 : 1;
            y = facing == Direction::UP ? 1 : -1;
            z = v->pos.z == 0 ? -1 : 1;

            s1Pos = (glm::ivec3)pos + glm::ivec3(x, y, 0);
            s2Pos = (glm::ivec3)pos + glm::ivec3(0, y, z);
            cPos = (glm::ivec3)pos + glm::ivec3(x, y, z);

        } else if (facing == Direction::EAST || facing == Direction::WEST) {
            x = facing == Direction::EAST ? 1 : -1;
            y = v->pos.y == 0 ? -1 : 1;
            z = v->pos.z == 0 ? -1 : 1;

            s1Pos = (glm::ivec3)pos + glm::ivec3(x, y, 0);
            s2Pos = (glm::ivec3)pos + glm::ivec3(x, 0, z);
            cPos = (glm::ivec3)pos + glm::ivec3(x, y, z);

        } else if (facing == Direction::SOUTH || facing == Direction::NORTH) {
            x = v->pos.x == 0 ? -1 : 1;
            y = v->pos.y == 0 ? -1 : 1;
            z = facing == Direction::SOUTH ? 1 : -1;

            s1Pos = (glm::ivec3)pos + glm::ivec3(x, 0, z);
            s2Pos = (glm::ivec3)pos + glm::ivec3(0, y, z);
            cPos = (glm::ivec3)pos + glm::ivec3(x, y, z);
        }

        int side1 = 0;
        if (s1Pos.y >= 0 && s1Pos.y < Chunk::CHUNK_HEIGHT) {
            if (Block::blockDefs[nearChunks.getBlock(s1Pos)].causesAO) side1 = 1;
        }
        int side2 = 0;
        if (s2Pos.y >= 0 && s2Pos.y < Chunk::CHUNK_HEIGHT) {
            if (Block::blockDefs[nearChunks.getBlock(s2Pos)].causesAO) side2 = 1;
        }
        int corner = 0;
        if (cPos.y >= 0 && cPos.y < Chunk::CHUNK_HEIGHT) {
            if (Block::blockDefs[nearChunks.getBlock(cPos)].causesAO) corner = 1;
        }

        if (side1 && side2) {
            v->aoValue = 0;
        } else {
            v->aoValue = 3 - (side1 + side2 + corner);
        }
    }
}

bool ChunkMesh::shouldAddFace(BlockId thisBlock, BlockId otherBlock) const {
    if (thisBlock && !otherBlock) return true;
    if (Block::isTransparent(thisBlock) && Block::isTransparent(otherBlock)) {
        if (thisBlock != otherBlock) return true;
    }
    if (thisBlock && !Block::isTransparent(thisBlock) && Block::isTransparent(otherBlock))
        return true;

    return false;
}

ChunkMesh::ChunkMesh(const Chunk& _chunk, World& world) : chunk(_chunk), nearChunks(_chunk, world) {
    createMesh(world);

    opaqueMesh.createBuffers();
    transparentMesh.createBuffers();
}

ChunkMesh::~ChunkMesh() {
    // LOG("Deleted mesh");
    opaqueMesh.deleteBuffers();
    transparentMesh.deleteBuffers();
}

void ChunkMesh::render(const Camera& camera, float aspectRatio) const {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearClip, camera.farClip);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(chunk.worldIndex.x, 0, chunk.worldIndex.y) * (float)Chunk::CHUNK_SIZE
    );

    // Shader
    const Shader& shaderValue = shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)));
    GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniform1f(opacityLoc, 1.0f));

    // Render
    glEnable(GL_CULL_FACE);
    opaqueMesh.render();
    glDisable(GL_CULL_FACE);
    transparentMesh.render();
}

size_t ChunkMesh::getSize() const {
    return opaqueMesh.vertices.size() + transparentMesh.vertices.size();
}

ChunkMesh::BorderingChunks::BorderingChunks(const Chunk& middleChunk, World& world) : chunks(9) {
    chunks[4] = &middleChunk;

    size_t i = 0;
    for (int z = -1; z <= 1; z++) {
        for (int x = -1; x <= 1; x++) {
            if (i != 4) {
                glm::ivec2 worldIdx = middleChunk.worldIndex + glm::ivec2(x, z);
                chunks[i] = &world.generateChunk(worldIdx, false).first->second;
            }
            i++;
        }
    }
}

BlockId ChunkMesh::BorderingChunks::getBlock(glm::ivec3 chunkPos) const {
    glm::ivec2 chunkOffset = glm::ivec2(0);

    if (chunkPos.x < 0) chunkOffset.x = -1;
    if (chunkPos.x >= Chunk::CHUNK_SIZE) chunkOffset.x = 1;

    if (chunkPos.z < 0) chunkOffset.y = -1;
    if (chunkPos.z >= Chunk::CHUNK_SIZE) chunkOffset.y = 1;

    size_t borderingIdx = 4;
    borderingIdx += chunkOffset.x;
    borderingIdx += chunkOffset.y * 3;

    chunkPos =
        glm::ivec3(chunkPos.x % Chunk::CHUNK_SIZE, chunkPos.y, chunkPos.z % Chunk::CHUNK_SIZE);
    if (chunkPos.x < 0) chunkPos.x += Chunk::CHUNK_SIZE;
    if (chunkPos.z < 0) chunkPos.z += Chunk::CHUNK_SIZE;

    if (chunkPos.x < 0 || chunkPos.z < 0) {
        std::cout << borderingIdx << " | " << chunkPos.x << ", y, " << chunkPos.z << "\n";
    }

    // if (chunks[borderingIdx] == nullptr) {
    //     for (auto i : chunks) {
    //         std::cout << i << "\n";
    //     }
    // }

    return chunks[borderingIdx]->getBlock(chunkPos);
}

const Chunk* const ChunkMesh::BorderingChunks::getMiddle() {
    return chunks[4];
}

void ChunkMesh::Mesh::createBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * vertexSize, &vertices[0], GL_STATIC_DRAW);

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

    // Texture index
    glVertexAttribPointer(
        3, 1, GL_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, textureIdx))
    );
    glEnableVertexAttribArray(3);

    // AO value
    glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, aoValue)));
    glEnableVertexAttribArray(4);

    // isLowered
    glVertexAttribPointer(
        5, 1, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, isLowered))
    );
    glEnableVertexAttribArray(5);
}

void ChunkMesh::Mesh::deleteBuffers() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ChunkMesh::Mesh::render() const {
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));
}
