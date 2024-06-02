#include "World.hpp"
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/hash.hpp>

World::World() {
    int dist = 2;
    for (int z = -dist; z < dist + 1; z++) {
        for (int x = -dist; x < dist + 1; x++) {
            const Chunk chunk = Chunk();
            auto idx = glm::ivec2(x, z);
            chunks[idx] = chunk;
            chunkMeshes.insert({idx, ChunkMesh(chunk)});
        }
    }
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
