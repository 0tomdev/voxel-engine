#pragma once

#include <glm/glm.hpp>

#include "../Shader.hpp"
#include "../block/Block.hpp"
#include "ChunkMesh.hpp"

class ChunkMesh;

class Chunk {
public:
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static const int CHUNK_ARRAY_SIZE = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    static glm::ivec2 getWorldIndex(glm::vec3 worldPos);
    static bool inBounds(glm::ivec3 chunkPos);

    const glm::ivec2 worldIndex;

    std::vector<BlockId> data;
    // I had to use a raw pointer 😭
    std::unique_ptr<ChunkMesh> mesh;

    Chunk(glm::ivec2 worldIndex);
    ~Chunk();

    // Move constructor (this causes a compiler warning but there's an error if I delete it)
    Chunk(Chunk&& other) noexcept = default;
    Chunk& operator=(Chunk&& other) noexcept = default;

    // Delete copy constructor and copy assignment operator
    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    // I just realized these should all be uvec3, not ivec3
    int getIndex(glm::ivec3 pos) const;
    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
    glm::ivec3 getWorldPosition(glm::ivec3 chunkPos) const;
    glm::ivec3 getChunkPosition(glm::ivec3 worldPos) const;
};
