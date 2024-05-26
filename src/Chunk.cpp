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
    data[4] = 1;
    data[5] = 1;
    data[6] = 1;
    data[5 + CHUNK_SIZE * CHUNK_SIZE] = 1;
    data[5 + CHUNK_SIZE] = 1;
    data[6 + CHUNK_SIZE] = 1;
    data[7 + CHUNK_SIZE] = 1;
    // for (int i = 0; i < CHUNK_SIZE; i++) {
    //     std::cout << (int)data[i] << " ";
    // }
    // std::cout << "\n";

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &buffer);
    std::cout << "chunk vbo: " << buffer << "\n";
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

void Chunk::render(Camera camera) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
        100.0f
    );

    // Shader
    const Shader& shaderValue = shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)));
    GL_CALL(glUniform1f(opacityLoc, 0.1f));

    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    int i = 0;
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x, y, z));
                // Render
                if (data[i]) {
                    GL_CALL(
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model))
                    );
                    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, allCubeVertices.size()));
                }
                i++;
            }
        }
    }
}