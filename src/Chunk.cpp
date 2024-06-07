#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"
#include "utils.hpp"

glm::ivec2 Chunk::getWorldIndex(glm::vec3 worldPos) {
    return glm::floor(glm::vec2(worldPos.x, worldPos.z) / (float)CHUNK_SIZE);
}

glm::ivec3 Chunk::getWorldPosition(glm::ivec2 worldIndex, glm::ivec3 chunkPos) {
    return glm::ivec3(worldIndex.x, 0, worldIndex.y) * CHUNK_SIZE + chunkPos;
}

Chunk::Chunk() : data(CHUNK_ARRAY_SIZE) {
    memset(data.data(), 0, CHUNK_ARRAY_SIZE);
}

Chunk::~Chunk() {}

int Chunk::getIndex(glm::ivec3 pos) const {
    int idx = pos.y * CHUNK_SIZE * CHUNK_SIZE;
    idx += pos.z * CHUNK_SIZE;
    idx += pos.x;
    return idx;
}

BlockId Chunk::getBlock(glm::ivec3 pos) const {
    int idx = getIndex(pos);
    assert(idx >= 0 && idx < CHUNK_ARRAY_SIZE);
    return data[idx];
}

void Chunk::setBlock(glm::ivec3 pos, BlockId block) {
    int idx = getIndex(pos);
    data[idx] = block;
}
