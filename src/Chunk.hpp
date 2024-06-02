#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "block/Block.hpp"

class Chunk {
public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static const int CHUNK_ARRAY_SIZE = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    // TODO: remove these
    static std::optional<Shader> shader;
    static unsigned int vertexColorLocation;
    static unsigned int modelLoc;
    static unsigned int viewLoc;
    static unsigned int projectionLoc;
    static unsigned int opacityLoc;

    static void init();

    static glm::ivec2 getChunkWorldIndex(glm::vec3 pos);

    // const glm::ivec2 worldIndex;
    // BlockId* data;
    std::vector<BlockId> data;

    Chunk();
    ~Chunk();

    int getIndex(glm::ivec3 pos) const;
    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
};