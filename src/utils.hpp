#pragma once

#include <iostream>
#include <chrono>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// #define GL_CALL(x)                                                                       \
//     clearGlErrors();                                                                     \
//     x;                                                                                   \
//     printGlErrors(#x)

#define GL_CALL(x) x

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

template <typename T>
T getRandom(T min, T max) {
    // Create a random device to seed the generator
    std::random_device rd;

    // Use the Mersenne Twister engine seeded with the random device
    std::mt19937 gen(rd());

    // Define the distribution range
    std::uniform_real_distribution<T> dis(min, max);

    // Generate the random number
    return dis(gen);
}

} // namespace utils