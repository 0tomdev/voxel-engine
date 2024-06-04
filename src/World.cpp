#include <glm/gtc/noise.hpp>

#include "World.hpp"

World::World() {
    // for (int z = -renderDistance; z < renderDistance + 1; z++) {
    //     for (int x = -renderDistance; x < renderDistance + 1; x++) {
    //         auto idx = glm::ivec2(x, z);
    //         generateChunk(idx);
    //     }
    // }
}

BlockId World::getBlock(glm::ivec3 worldPos) const {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    const Chunk& c = chunks.at(chunkWorldIdx);
    return c.getBlock(worldPos % Chunk::CHUNK_SIZE);
}

void World::setBlock(glm::ivec3 worldPos, BlockId block) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
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

void World::update() {
    // Generate and load chunks around player
    glm::ivec2 worldIdx = Chunk::getWorldIndex(player.position);
    for (int z = worldIdx.y - renderDistance; z < worldIdx.y + 1; z++) {
        for (int x = worldIdx.x - renderDistance; x < worldIdx.x + 1; x++) {
            auto idx = glm::ivec2(x, z);
            if (generateChunk(idx)) {
                std::cout << "Generated chunk (" << x << ", " << z << ")\n";
            }
        }
    }
}

bool World::generateChunk(glm::ivec2 worldIdx) {
    Chunk chunk = Chunk();
    auto [it, wasInserted] = chunks.insert({worldIdx, chunk});

    if (!wasInserted) return false;

    for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
            auto worldPos = Chunk::getWorldPosition(worldIdx, glm::ivec3(x, 0, z));

            float height = glm::perlin(glm::vec2(worldPos.x, worldPos.z) / 32.0f);
            height = utils::mapValue(height, -1.0f, 1.0f, 80.0f, 100.0f);

            int intHeight = height;

            for (int cy = 0; cy < intHeight; cy++) {
                BlockId b = cy == intHeight - 1 ? Block::GRASS : Block::STONE;
                it->second.setBlock({x, cy, z}, b);
            }
        }
    }
    chunkMeshes.insert({worldIdx, ChunkMesh(it->second)}); // temporary

    return true;
}
