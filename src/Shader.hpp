#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragPath);
};