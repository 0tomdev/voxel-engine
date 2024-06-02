#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Chunk.hpp"
#include "utils.hpp"
#include "ChunkMesh.hpp"
#include "World.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

bool mouseCaptured = true;
bool wireFrameMode = false;
bool firstMouse = true;

int screenWidth = 1200;
int screenHeight = 800;

Camera camera;

glm::vec2 mousePos = glm::vec2(screenWidth, screenHeight) / 2.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        mouseCaptured = !mouseCaptured;
        if (mouseCaptured) firstMouse = true;
        glfwSetInputMode(
            window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );
    } else if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        wireFrameMode = !wireFrameMode;
    }
}
// test

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // std::cout << xpos << " " << ypos << "\n";
    if (!mouseCaptured) return;
    if (firstMouse) {
        mousePos = glm::vec2(xpos, ypos);
        firstMouse = false;
    }

    glm::vec2 offset = glm::vec2(xpos, ypos) - mousePos;
    offset.y *= -1; // reversed since y-coordinates range from bottom to top

    camera.updateFromMouse(offset);

    mousePos = glm::vec2(xpos, ypos);
}

void resizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
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
        glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL, NULL);
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

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    utils::Texture texture("./assets/textures/texture_atlas.png");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glm::mat4 projection = glm::perspective(
    //     glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
    //     100.0f
    // );

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Chunk::init();
    Block::initBlocks();

    World world;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        const float cameraSpeed = 5 * deltaTime;

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

        glPolygonMode(
            GL_FRONT_AND_BACK, wireFrameMode ? GL_LINE : GL_FILL
        ); // Wireframe mode

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();

        camera.calculateDirection();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        world.render(camera, screenWidth / (float)screenHeight);

        {
            ImGui::Begin("Debug Info");

            ImGui::BeginDisabled(mouseCaptured);

            ImGui::Text("FPS: %f", 60.0f / deltaTime);
            ImGui::DragFloat("Position x", &camera.position.x, 1, -100, 100);
            ImGui::DragFloat("Position y", &camera.position.y, 1, 0, 255);
            ImGui::DragFloat("Position z", &camera.position.z, 1, -100, 100);
            ImGui::Checkbox("Wireframe mode", &wireFrameMode);

            ImGui::BeginChild("Chunk Info");
            glm::ivec2 chunkIdx = Chunk::getChunkWorldIndex(camera.position);
            ImGui::Text("World index: (%i, %i)", chunkIdx.x, chunkIdx.y);
            // ImGui::Text(
            //     "Mesh size: %i kb", mesh.getSize() * ChunkMesh::vertexSize / 1024
            // );

            ImGui::EndChild();

            ImGui::EndDisabled();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}