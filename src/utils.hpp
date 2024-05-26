#pragma once

#include <iostream>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GL_CALL(x)                                                                       \
    clearGlErrors();                                                                     \
    x;                                                                                   \
    printGlErrors(#x)

// #define GL_CALL(x) x

void clearGlErrors();
void printGlErrors(const char* function);

namespace utils {

enum Direction { EAST, WEST, UP, DOWN, SOUTH, NORTH };

struct Texture {
    unsigned int id; // OpenGL id
    int width, height;
    int nrChannels;

    Texture(const char* imagePath);
};

class ScopeTimer {
public:
    ScopeTimer();
    ~ScopeTimer();

private:
    const char* message;
    std::chrono::time_point<std::chrono::steady_clock> start;
};

} // namespace utils