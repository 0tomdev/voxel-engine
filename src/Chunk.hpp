#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "block/Block.hpp"

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

    BlockId* data;

    Chunk();
    ~Chunk();

    int getIndex(glm::ivec3 pos) const;
    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
};