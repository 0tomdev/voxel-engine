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
 *  |2------+3
 *  ||      ||
 *  ||      ||
 *  4+------5|
 *   6-------7
 *
 * TOP
 * 0-------1
 * |       |
 * |       |
 * |       |
 * 2-------3
 *
 * BOTTOM
 * 4-------5
 * |       |
 * |       |
 * |       |
 * 6-------7
 *
 * east (+x) -->
 * up (+y) ⬆
 * south (+z) into your face
 * north (-z) into the screen
 *
 */

// clang-format off
static float cubeVertices[] {
    0, 1, 0,
    1, 1, 0,
    0, 1, 1,
    1, 1, 1,

};

static const unsigned int cubeIndices[] {
    0, 1, 2, // front
    1, 2, 3,
    4, 5, 6, // back
    5, 6, 7,
    1, 5, 7, // right
    1, 7, 3,
    0, 4, 6, // left
    0, 2, 6,
    0, 4, 1, // top
    4, 1, 5,
    2, 6, 3, // bottom
    6, 7, 3
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