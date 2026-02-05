#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/noise.hpp>

#include "utils.hpp"

void clearGlErrors() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

void printGlErrors(const char* function) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL error: 0x" << std::hex << error << "\n";
        std::cout << "\t" << function << std::endl;
    }
}

utils::Texture::Texture(const char* filePath) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, STBI_rgb_alpha);
    std::cout << "Image loaded: " << width << " " << height << " " << nrChannels << "\n";

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    id = texture;
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
}

utils::ScopeTimer::ScopeTimer() {
    // message = msg;
    start = std::chrono::steady_clock::now();
}
utils::ScopeTimer::~ScopeTimer() {
    const auto end = std::chrono::steady_clock::now();
    const auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Timer took " << durationMs.count() << "ms\n";
}

glm::ivec3 utils::getDirectionOffset(Direction dir) {
    glm::ivec3 result = glm::ivec3(0);

    switch (dir) {
        case EAST: result.x = 1; break;
        case WEST: result.x = -1; break;
        case UP: result.y = 1; break;
        case DOWN: result.y = -1; break;
        case SOUTH: result.z = 1; break;
        case NORTH: result.z = -1; break;
    }

    return result;
}

float utils::octiveNoise(glm::vec2 value, uint32_t octives, float lacunarity, float persistence) {
    float result = 0;

    float ampl = 1;
    float freq = 1;
    for (int i = 0; i < octives; i++) {
        result += ampl * glm::simplex(value * freq);
        freq *= lacunarity;
        ampl /= persistence;
    }

    return result;
}
