#pragma once

#include <iostream>
#include <chrono>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define DEBUG 1

// #define GL_CALL(x)                                                                       \
//     clearGlErrors();                                                                     \
//     x;                                                                                   \
//     printGlErrors(#x)

#define GL_CALL(x) x

#if DEBUG == 1
#define SCOPE_TIMER(x) utils::ScopeTimer x
#define LOG(x) std::cout << x << "\n"
#else
#define SCOPE_TIMER(x)
#define LOG(x)
#endif

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
    std::chrono::time_point<std::chrono::steady_clock> start;
};

template <typename T>
T getRandom(T min, T max) {
    static std::random_device rd;  // Static to be initialized only once
    static std::mt19937 gen(rd()); // Static to be initialized only once

    if constexpr (std::is_integral<T>::value) {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(gen);
    } else {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(gen);
    }
}

// Thanks ChatGPT 😋
template <typename T>
T mapValue(T value, T inputMin, T inputMax, T outputMin, T outputMax) {
    return outputMin + (value - inputMin) * (outputMax - outputMin) / (inputMax - inputMin);
}

/**
 * @param lacunarity adjusts frequency
 * @param persistance adjusts amplitude
 */
float octiveNoise(glm::vec2 value, uint32_t octives, float lacunarity = 2, float persistence = 2);

struct Ray {
    glm::vec3 origin, direction;
    float length;
};

} // namespace utils