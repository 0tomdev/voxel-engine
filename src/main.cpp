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

// Too big for stack :(
std::unique_ptr<World> world;

glm::vec2 mousePos = glm::ivec2(screenWidth, screenHeight) / 2;

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // std::cout << xpos << " " << ypos << "\n";
    if (!mouseCaptured) return;
    if (firstMouse) {
        mousePos = glm::vec2(xpos, ypos);
        firstMouse = false;
    }

    glm::vec2 offset = glm::vec2(xpos, ypos) - mousePos;
    offset.y *= -1; // reversed since y-coordinates range from bottom to top

    world->player.updateCameraFromMouse(offset);

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

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", NULL, NULL);
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

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ChunkMesh::init();
    Block::initBlocks();

    world = std::make_unique<World>();

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
        static float cameraSpeed = 15;
        const float camSpeedAdjusted = cameraSpeed * deltaTime;

        Camera& cam = world->player.camera;

        glm::vec3 newCamFront = cam.direction;
        newCamFront.y = 0;
        newCamFront = glm::normalize(newCamFront);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            world->player.position += camSpeedAdjusted * newCamFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            world->player.position -= camSpeedAdjusted * newCamFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            world->player.position -=
                glm::normalize(glm::cross(newCamFront, cam.getUp())) * camSpeedAdjusted;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            world->player.position +=
                glm::normalize(glm::cross(newCamFront, cam.getUp())) * camSpeedAdjusted;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            world->player.position += camSpeedAdjusted * glm::vec3(0, 1, 0);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            world->player.position += camSpeedAdjusted * glm::vec3(0, -1, 0);

        // Logic
        world->update();

        // Rendering
        glClearColor(98 / 255.0f, 162 / 255.0f, 245 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wireFrameMode ? GL_LINE : GL_FILL); // Wireframe mode

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        // cam.position = world->player.position;
        world->render(cam, screenWidth / (float)screenHeight);

        {
            ImGui::Begin("Debug Info");

            ImGui::BeginDisabled(mouseCaptured);

            ImGui::Text("FPS: %i", (int)(1.0f / deltaTime));
            ImGui::DragFloat("Position x", &world->player.position.x, 1, -100, 100);
            ImGui::DragFloat("Position y", &world->player.position.y, 1, 0, 255);
            ImGui::DragFloat("Position z", &world->player.position.z, 1, -100, 100);
            ImGui::Checkbox("Wireframe mode", &wireFrameMode);
            ImGui::SliderFloat("Movement speed", &cameraSpeed, 5.0f, 30.0f);

            ImGui::SeparatorText("Chunk Data");
            glm::ivec2 chunkIdx = Chunk::getWorldIndex(world->player.position);
            ImGui::Text("World index: (%i, %i)", chunkIdx.x, chunkIdx.y);
            ImGui::Text(
                "Total chunk memory usage %i kb",
                world->getNumChunks() * Chunk::CHUNK_ARRAY_SIZE * sizeof(BlockId) / 1000
            );
            // ImGui::Text(
            //     "Mesh size: %i kb", mesh.getSize() * ChunkMesh::vertexSize / 1024
            // );

            // ImGui::SeparatorText("World Editing");
            // static glm::ivec3 pos;
            // static int blockId;
            // ImGui::InputInt3("Block position", &pos.x);
            // ImGui::InputInt("Block ID", &blockId);
            // auto idx = glm::ivec2(0, 0);
            // if (ImGui::Button("Place Block")) {
            //     auto it = world.chunks.find(idx);
            //     if (it != world.chunks.end()) {
            //         it->second.setBlock(pos, Block::STONE_BRICKS);
            //     }
            // }
            // if (ImGui::Button("Remesh")) {
            //     auto it = world.chunks.find(idx);
            //     if (it != world.chunks.end()) {
            //         world.chunkMeshes.at(idx) = ChunkMesh(it->second);
            //     }
            // }

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