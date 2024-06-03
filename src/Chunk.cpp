#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"
#include "utils.hpp"

std::optional<Shader> Chunk::shader;
unsigned int Chunk::vertexColorLocation;
unsigned int Chunk::modelLoc;
unsigned int Chunk::viewLoc;
unsigned int Chunk::projectionLoc;
unsigned int Chunk::opacityLoc;

/**
 * Load shaders here because GL functions can only be called after glewInit()
 * */
void Chunk::init() {
    Chunk::shader = Shader("./assets/shaders/vertex.glsl", "./assets/shaders/frag.glsl");
    const Shader& shaderValue = shader.value();
    Chunk::vertexColorLocation = glGetUniformLocation(shaderValue.ID, "time");
    Chunk::modelLoc = glGetUniformLocation(shaderValue.ID, "model");
    Chunk::viewLoc = glGetUniformLocation(shaderValue.ID, "view");
    Chunk::projectionLoc = glGetUniformLocation(shaderValue.ID, "projection");
    Chunk::opacityLoc = glGetUniformLocation(shaderValue.ID, "opacity");
}

glm::ivec2 Chunk::getChunkWorldIndex(glm::vec3 pos) {
    return glm::floor(glm::vec2(pos.x, pos.z) / (float)CHUNK_SIZE);
}

glm::ivec3 Chunk::getWorldPosition(glm::ivec2 worldIndex, glm::ivec3 chunkPos) {
    return glm::ivec3(worldIndex.x, 0, worldIndex.y) * CHUNK_SIZE + chunkPos;
}

Chunk::Chunk() : data(CHUNK_ARRAY_SIZE) {
    int height = utils::getRandom<int>(150, 200);
    // std::cout << "Height: " << height << "\n";
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
