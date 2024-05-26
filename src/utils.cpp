#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    id = texture;
    if (data) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);
}

utils::ScopeTimer::ScopeTimer() {
    // message = msg;
    start = std::chrono::high_resolution_clock::now();
}
utils::ScopeTimer::~ScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Timer took " << durationMs.count() << "ms\n";
}
