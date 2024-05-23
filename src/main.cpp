#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Chunk.hpp"
#include "global.hpp"
#include "utils.hpp"

bool mouseCaptured = true;

Camera camera;

glm::vec2 mousePos = glm::vec2(Global::screenWidth, Global::screenHeight) / 2.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        mouseCaptured = !mouseCaptured;
        glfwSetInputMode(
            window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );
    }
}
// test

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // std::cout << xpos << " " << ypos << "\n";
    if (!mouseCaptured) return;

    glm::vec2 offset = glm::vec2(xpos, ypos) - mousePos;
    offset.y *= -1; // reversed since y-coordinates range from bottom to top

    camera.updateFromMouse(offset);

    mousePos = glm::vec2(xpos, ypos);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize glfw\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac ig

    GLFWwindow* window =
        glfwCreateWindow(Global::screenWidth, Global::screenHeight, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create OpenGL window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetWindowSizeCallback(
    //     window,
    //     [](GLFWwindow* window, int newWidth, int newHeight) {
    //         Global::screenWidth = newWidth;
    //         Global::screenHeight = newHeight;
    //     }
    // );

    utils::Texture texture("./assets/textures/diamond_ore.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glm::mat4 projection = glm::perspective(
    //     glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
    //     100.0f
    // );

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Chunk::init();

    Chunk chunk;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        const float cameraSpeed = 2.5 * deltaTime;

        glm::vec3 newCamFront = camera.getDirection();
        newCamFront.y = 0;
        newCamFront = glm::normalize(newCamFront);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.position += cameraSpeed * newCamFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.position -= cameraSpeed * newCamFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.position -=
                glm::normalize(glm::cross(newCamFront, camera.getUp())) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.position +=
                glm::normalize(glm::cross(newCamFront, camera.getUp())) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.position += cameraSpeed * glm::vec3(0, 1, 0);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.position += cameraSpeed * glm::vec3(0, -1, 0);

        // Rendering
        glClearColor(98 / 255.0f, 162 / 255.0f, 245 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.calculateDirection();

        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        chunk.render(camera);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}