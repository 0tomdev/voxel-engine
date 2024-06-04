#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "block/Block.hpp"

class Chunk {
public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static const int CHUNK_ARRAY_SIZE = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    static glm::ivec2 getWorldIndex(glm::vec3 worldPos);
    static glm::ivec3 getWorldPosition(glm::ivec2 worldIndex, glm::ivec3 chunkPos);

    // const glm::ivec2 worldIndex;
    // BlockId* data;
    std::vector<BlockId> data;

    Chunk();
    ~Chunk();

    int getIndex(glm::ivec3 pos) const;
    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
};