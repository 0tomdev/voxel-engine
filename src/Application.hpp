#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Application {
public:
    Application();

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};