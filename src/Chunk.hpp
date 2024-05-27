#pragma once

#include <glm/glm.hpp>

#include "types.hpp"
#include "Shader.hpp"

class Chunk {
public:
    enum Direction { EAST, WEST, UP, DOWN, SOUTH, NORTH };

    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static const int CHUNK_ARRAY_SIZE = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    static std::optional<Shader> shader;
    static unsigned int vertexColorLocation;
    static unsigned int modelLoc;
    static unsigned int viewLoc;
    static unsigned int projectionLoc;
    static unsigned int opacityLoc;

    static void init();

    BlockID* data;

    Chunk();
    ~Chunk();

    int getIndex(glm::vec3 pos) const;
    BlockID getBlock(glm::vec3 pos) const;
    void setBlock(glm::vec3 pos, BlockID block);
};