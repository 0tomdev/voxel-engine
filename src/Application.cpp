#include "Application.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.hpp"
#include "Camera.hpp"
#include "world/Chunk.hpp"
#include "utils.hpp"
#include "world/ChunkMesh.hpp"
#include "world/World.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static void drawRect(glm::ivec2 pos, glm::ivec2 size, const utils::Texture& texture) {
    struct Vertex {
        glm::ivec2 pos;
        glm::vec2 texCoords;
        Vertex(int x, int y) : pos(x, y), texCoords(x, y) {};
    };
    std::vector<Vertex> verts = {{0, 1}, {1, 1}, {0, 0}, {1, 0}};
    for (Vertex& v : verts) {
        v.pos = v.pos * size + pos;
    }

    std::vector<unsigned int> indices = {0, 1, 2, 3, 2, 1};
    size_t vertSize = sizeof(Vertex);

    // Create buffers
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * vertSize, &verts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, vertSize, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertSize, (void*)(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(1);

    // Render
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Delete buffers
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
};

Application Application::instance;

Application& Application::get() {
    return instance;
}

Application::Application() {}

void Application::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    Application& app = get();

    if (!app.mouse.isCaptured) return;
    if (app.mouse.firstMove) {
        app.mouse.position = glm::vec2(xpos, ypos);
        app.mouse.firstMove = false;
    }

    glm::vec2 offset = glm::ivec2(xpos, ypos) - app.mouse.position;
    offset.y *= -1; // reversed since y-coordinates range from bottom to top

    app.world->player.updateCameraFromMouse(offset);

    app.mouse.position = glm::vec2(xpos, ypos);
}

void Application::onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application& app = get();

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app.mouse.isCaptured = !app.mouse.isCaptured;
        if (app.mouse.isCaptured) app.mouse.firstMove = true;
        glfwSetInputMode(
            window, GLFW_CURSOR, app.mouse.isCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
        );
    } else if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        app.wireFrameMode = !app.wireFrameMode;
    }
}

void Application::onWindowResize(GLFWwindow* window, int newWidth, int newHeight) {
    Application& app = get();

    app.windowSize = glm::uvec2(newWidth, newHeight);
}

void Application::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
    Application& app = get();

    app.mouse.buttons[button].isDown = action == GLFW_PRESS;
    if (action == GLFW_PRESS) {
        app.mouse.buttons[button].wasPressed = true;
    }
}

void Application::onGlfwError(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

GLFWwindow* Application::getWindow() const {
    return window;
}

const Mouse& Application::getMouse() const {
    return mouse;
}

const Keyboard& Application::getKeyboard() const {
    return keyboard;
}

bool Application::init() {
    glfwSetErrorCallback(onGlfwError);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac ig

    window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create OpenGL window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW\n";
        return false;
    }

    glfwSetKeyCallback(window, onKeyAction);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetWindowSizeCallback(window, onWindowResize);
    glfwSetMouseButtonCallback(window, onMouseButton);

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    textures.emplace("texture_atlas", utils::Texture("./assets/textures/texture_atlas.png"));
    textures.emplace("crosshair", utils::Texture("./assets/textures/crosshair.png"));

    shaders.emplace("chunk", Shader("./assets/shaders/chunk.vert", "./assets/shaders/chunk.frag"));
    shaders.emplace("2d", Shader("./assets/shaders/2d.vert", "./assets/shaders/2d.frag"));

    GLuint screenSizeLoc = glGetUniformLocation(shaders.at("2d").ID, "screenSize");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    return true;
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Logic
        world->update(deltaTime);

        // Rendering
        glClearColor(98 / 255.0f, 162 / 255.0f, 245 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wireFrameMode ? GL_LINE : GL_FILL); // Wireframe mode

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();

        world->render(windowSize.x / (float)windowSize.y);

        // Render UI
        const auto& shader2D = shaders.at("2d");
        glUseProgram(shader2D.ID);
        glm::uvec2 screenSize(windowSize.x, windowSize.y);
        glUniform2uiv(shader2D.getUniformLocation("screenSize"), 1, &screenSize.x);
        uint32_t crosshairSize = textures.at("crosshair").width;
        crosshairSize = 12;
        drawRect(
            ((glm::ivec2)screenSize - glm::ivec2(crosshairSize)) / 2, glm::ivec2(crosshairSize),
            textures.at("crosshair")
        );

        {
            ImGui::Begin("Debug Info");

            ImGui::BeginDisabled(mouse.isCaptured);

            ImGui::Text("FPS: %i", (int)(1.0f / deltaTime));
            ImGui::DragFloat("Position x", &world->player.position.x, 1, -100, 100);
            ImGui::DragFloat("Position y", &world->player.position.y, 1, 0, 255);
            ImGui::DragFloat("Position z", &world->player.position.z, 1, -100, 100);
            ImGui::Checkbox("Wireframe mode", &wireFrameMode);
            ImGui::SliderFloat("Movement speed", &world->player.movementSpeed, 5.0f, 30.0f);

            ImGui::SeparatorText("Chunk Data");
            glm::ivec2 chunkIdx = Chunk::getWorldIndex(world->player.position);
            ImGui::Text("World index: (%i, %i)", chunkIdx.x, chunkIdx.y);
            ImGui::Text(
                "Total chunk memory usage %i kb",
                world->getNumChunks() * Chunk::CHUNK_ARRAY_SIZE * sizeof(BlockId) / 1000
            );

            // Need to handle error for this line
            const ChunkMesh* mesh = world->chunks.at(chunkIdx).mesh.get();

            if (mesh) {
                ImGui::SeparatorText("Mesh Data");
                ImGui::Text("Mesh size: %i kb", mesh->getSize() * ChunkMesh::vertexSize / 1024);
                ImGui::Text("Generation time: %i ms", mesh->generationTime);
            }

            ImGui::EndDisabled();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        for (InputButton& btn : mouse.buttons) {
            btn.wasPressed = false;
        }
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::shutDown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}
