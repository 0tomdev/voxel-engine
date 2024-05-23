#include <iostream>

namespace utils {
struct Texture {
    unsigned int id; // OpenGL id
    int width, height;
    int nrChannels;

    Texture(const char* imagePath);
};
} // namespace utils