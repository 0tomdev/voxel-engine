#pragma once

#include <iostream>
#include <chrono>

namespace utils {
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