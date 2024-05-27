#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"
#include "global.hpp"
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

Chunk::Chunk() {
    data = new BlockID[CHUNK_ARRAY_SIZE];
    memset(data, 0, CHUNK_ARRAY_SIZE);
    memset(data, 1, CHUNK_ARRAY_SIZE / 2 + 107);
    // setBlock({ 8, 5, 8 }, 0);
}

Chunk::~Chunk() {
    delete[] data;
}

int Chunk::getIndex(glm::vec3 pos) const {
    int idx = pos.y * CHUNK_SIZE * CHUNK_SIZE;
    idx += pos.z * CHUNK_SIZE;
    idx += pos.x;
    return idx;
}

BlockID Chunk::getBlock(glm::vec3 pos) const {
    int idx = getIndex(pos);
    assert(idx >= 0 && idx < CHUNK_ARRAY_SIZE);
    return data[idx];
}

void Chunk::setBlock(glm::vec3 pos, BlockID block) {
    int idx = getIndex(pos);
    data[idx] = block;
}
