#pragma once

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragPath);

    GLint getUniformLocation(const std::string& name) const;

private:
    mutable std::unordered_map<std::string, GLint> uniformLocations;
};