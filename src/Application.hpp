#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>

#include "world/World.hpp"

struct InputButton {
    // True if currently down
    bool isDown;
    // True if pressed this frame
    bool wasPressed;
    // True of released this frame
    // bool wasReleased;
};

struct Mouse {
    InputButton buttons[GLFW_MOUSE_BUTTON_LAST + 1];
    glm::ivec2 position;
    bool isCaptured = true, firstMove = true;
};

struct Keyboard {
    InputButton keys[GLFW_KEY_LAST + 1];
};

class Application {
public:
    std::unordered_map<std::string, Shader> shaders;
    std::unordered_map<std::string, utils::Texture> textures;
    std::unique_ptr<World> world;

    static Application& get();

    Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Callbacks for GLFW
    static void onMouseMove(GLFWwindow* window, double xpos, double ypos);
    static void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onWindowResize(GLFWwindow* window, int newWidth, int newHeight);
    static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void onGlfwError(int error, const char* description);

    GLFWwindow* getWindow() const;
    const Mouse& getMouse() const;
    const Keyboard& getKeyboard() const;
    const glm::uvec2 getWindowSize() const;

    bool init();
    void run();
    void shutDown();

private:
    static Application instance;

    GLFWwindow* window;
    glm::uvec2 windowSize = {1200, 800};
    bool wireFrameMode = false;

    // Input state
    Mouse mouse;
    Keyboard keyboard;

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
};
