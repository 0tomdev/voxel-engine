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
static float cubeVertices[] {
    // Top
    0, 1, 0,
    1, 1, 0,
    0, 1, 1,
    1, 1, 1,

    // Bottom
    0, 0, 0,
    1, 0, 0,
    0, 0, 1,
    1, 0, 1,
};

static const unsigned int cubeIndices[] {
    // Top
    0, 1, 2,
    1, 2, 3,

    // Bottom
    4, 5, 6,
    5, 6, 7,

    // Left
    0, 2, 4,
    4, 2, 6,

    // Right
    1, 3, 5,
    5, 3, 7,

    // Front
    2, 3, 6,
    6, 3, 7,

    // Back
    0, 1, 4,
    4, 1, 5
};
// clang-format on

std::optional<Shader> Chunk::shader;
unsigned int Chunk::vertexColorLocation;
unsigned int Chunk::modelLoc;
unsigned int Chunk::viewLoc;
unsigned int Chunk::projectionLoc;
unsigned int Chunk::opacityLoc;
std::vector<Chunk::Vertex> Chunk::allCubeVertices = {
    { 1, 1, 0, 1, 1, 0 }, { 1, 1, 1, 0, 1, 0 }, { 1, 0, 0, 1, 0, 0 },

    { 1, 0, 1, 0, 0, 0 }, { 1, 0, 0, 1, 0, 0 }, { 1, 1, 1, 0, 1, 0 },

    { 0, 1, 1, 1, 1, 1 }, { 0, 1, 0, 0, 1, 1 }, { 0, 0, 0, 0, 0, 1 },

    { 0, 0, 0, 0, 0, 1 }, { 0, 0, 1, 1, 0, 1 }, { 0, 1, 1, 1, 1, 1 },

    { 1, 1, 0, 1, 1, 2 }, { 0, 1, 0, 0, 1, 2 }, { 0, 1, 1, 0, 0, 2 },

    { 0, 1, 1, 0, 0, 2 }, { 1, 1, 1, 1, 0, 2 }, { 1, 1, 0, 1, 1, 2 },

    { 0, 0, 0, 0, 0, 3 }, { 1, 0, 0, 1, 0, 3 }, { 0, 0, 1, 0, 1, 3 },

    { 1, 0, 1, 1, 1, 3 }, { 0, 0, 1, 0, 1, 3 }, { 1, 0, 0, 1, 0, 3 },

    { 1, 1, 1, 1, 1, 4 }, { 0, 1, 1, 0, 1, 4 }, { 0, 0, 1, 0, 0, 4 },

    { 0, 0, 1, 0, 0, 4 }, { 1, 0, 1, 1, 0, 4 }, { 1, 1, 1, 1, 1, 4 },

    { 0, 1, 0, 0, 1, 5 }, { 1, 1, 0, 1, 1, 5 }, { 0, 0, 0, 0, 0, 5 },

    { 1, 0, 0, 1, 0, 5 }, { 0, 0, 0, 0, 0, 5 }, { 1, 1, 0, 1, 1, 5 }
};

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

    return;
    // Generate actual cube vertices
    // garbage code
    for (int i = 0; i < 6; i++) {
        Direction dir = (Direction)i;
        std::vector<Vertex> vertices;
        int positionVecComponentOffset = i / 2;
        int namingIsHard = 1 - i % 2; // this will be 1 or 0
        std::vector<Vertex> faceVerts;
        for (int j = 0; j < 24; j += 3) {
            if (cubeVertices[j + positionVecComponentOffset] != namingIsHard) continue;
            Vertex vert;
            vert.x = cubeVertices[j];
            vert.y = cubeVertices[j + 1];
            vert.z = cubeVertices[j + 2];
            vert.normal = i;
            switch (positionVecComponentOffset) {
                case 0:
                    vert.u = vert.y;
                    vert.v = vert.z;
                    break;
                case 1:
                    vert.u = vert.x;
                    vert.v = vert.z;
                    break;
                case 2:
                    vert.u = vert.x;
                    vert.v = vert.y;
                    break;
                default: break;
            }

            faceVerts.push_back(vert);
        }
        int tris[] = { 0, 1, 2, 2, 3, 1 };
        assert(faceVerts.size() == 4);
        for (int k : tris) {
            allCubeVertices.push_back(faceVerts[k]);
        }
    }

    int i = 0;
    for (Vertex v : allCubeVertices) {
        std::cout << v.x << ", " << v.y << ", " << v.z << "   " << v.u << ", " << v.v
                  << "   " << (int)v.normal << "\n";
        if (i % 3 == 2) std::cout << "\n";
        i++;
    }
}

Chunk::Chunk() {
    data = new BlockID[CHUNK_ARRAY_SIZE];
    memset(data, 0, CHUNK_ARRAY_SIZE);
    memset(data, 1, CHUNK_ARRAY_SIZE - 70 - CHUNK_SIZE * CHUNK_SIZE * 3);
    // setBlock({ 8, 5, 8 }, 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &buffer);
    // glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBufferData(
        GL_ARRAY_BUFFER, allCubeVertices.size() * sizeof(Vertex), &allCubeVertices[0],
        GL_STATIC_DRAW
    );

    // xyz coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // uv coords
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u))
    );
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);
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
