#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"
#include "global.hpp"

#define GL_CALL(x)                                                                       \
    glClearErrors();                                                                     \
    x;                                                                                   \
    glPrintErrors(#x)

void glClearErrors() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

void glPrintErrors(const char* function) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL error: 0x" << std::hex << error << "\n";
        std::cout << "\t" << function << std::endl;
    }
}

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
 * VERTEX FORMAT
 * [x, y, x, u, v]
 *
 * (x, y, z) - world coords
 * (u, v) - texture coords
 */

// clang-format off
static float cubeVertices[] {
    // Top
    0, 1, 0,  0, 0,
    1, 1, 0,  1, 0,
    0, 1, 1,  0, 1,
    1, 1, 1,  1, 1,

    // Bottom
    0, 0, 0,  0, 0,
    1, 0, 0,  1, 0,
    0, 0, 1,  0, 1,
    1, 0, 1,  1, 1,
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
}

Chunk::Chunk() {
    memset(data, 1, CHUNK_SIZE * sizeof(BlockID));
    for (int i = 0; i < CHUNK_SIZE; i++) {
        std::cout << (int)data[i] << " ";
    }
    std::cout << "\n";

    unsigned int EBO; // element buffer object
    unsigned int buffer;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &buffer);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Chunk::render(Camera camera) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
        100.0f
    );

    float timeValue = glfwGetTime();

    // Shader
    const Shader& shaderValue = shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniform1f(vertexColorLocation, timeValue));
    GL_CALL(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)));

    // Render
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glDrawElements(
        GL_TRIANGLES, sizeof(cubeIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0
    ));
}