#include <glm/gtc/noise.hpp>

#include "World.hpp"

World::World() {
    // int dist = 0;
    // for (int z = -dist; z < dist + 1; z++) {
    //     for (int x = -dist; x < dist + 1; x++) {
    //         auto idx = glm::ivec2(x, z);
    //         generateChunk(idx);
    //     }
    // }
    generateChunk({0, 0});
}

BlockId World::getBlock(glm::ivec3 worldPos) const {
    glm::ivec2 chunkWorldIdx = Chunk::getChunkWorldIndex(worldPos);
    const Chunk& c = chunks.at(chunkWorldIdx);
    return c.getBlock(worldPos % Chunk::CHUNK_SIZE);
}

void World::setBlock(glm::ivec3 worldPos, BlockId block) {
    glm::ivec2 chunkWorldIdx = Chunk::getChunkWorldIndex(worldPos);
    Chunk& c = chunks.at(chunkWorldIdx);
    c.setBlock(worldPos % Chunk::CHUNK_SIZE, block);
}

void World::render(const Camera& camera, float aspectRatio) const {
    for (auto it = chunkMeshes.begin(); it != chunkMeshes.end(); it++) {
        it->second.render(camera, aspectRatio, it->first);
    }
}

size_t World::getNumChunks() const {
    return chunks.size();
}

void World::generateChunk(glm::ivec2 worldIdx) {
    Chunk chunk = Chunk();
    for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
            auto worldPos = Chunk::getWorldPosition(worldIdx, glm::ivec3(x, 0, z));

            float height = glm::perlin(glm::vec2(worldPos.x, worldPos.z) / 8.0f);
            std::cout << worldPos.x << " " << worldPos.z << "\n";
            // std::cout << height << "\n";
            height = utils::mapValue(height, -1.0f, 1.0f, 80.0f, 100.0f);

            int intHeight = height;

            for (int cy = 0; cy < intHeight; cy++) {
                BlockId b = cy == intHeight - 1 ? Block::GRASS : Block::STONE;
                chunk.setBlock({x, cy, z}, b);
            }
        }
    }
    chunks[worldIdx] = chunk;
    // if (worldIdx == glm::ivec2(0)) {
    //     for (auto i : chunk.data)
    //         std::cout << (int)i << " ";
    //     std::cout << "\n";
    // }
    chunkMeshes.insert({worldIdx, ChunkMesh(chunk)});
}
