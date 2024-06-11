#include <glm/gtc/noise.hpp>

#include "World.hpp"
#include <Instrumentor.h>

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

BlockId World::getBlockOrGenChunk(glm::ivec3 worldPos) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    auto result = generateChunk(worldPos, false);
    return result.first->second.getBlock(worldPos & Chunk::CHUNK_SIZE);
}

void World::setBlock(glm::ivec3 worldPos, BlockId block) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    Chunk& c = chunks.at(chunkWorldIdx);
    c.setBlock(worldPos % Chunk::CHUNK_SIZE, block);
}

void World::render(const Camera& camera, float aspectRatio) const {
    PROFILE_FUNCTION();

    std::vector<const Chunk*> chunksSorted(chunks.size());

    int i = 0;
    for (auto it = chunks.begin(); it != chunks.end(); it++) {
        chunksSorted[i] = &(it->second);
        i++;
    }

    // Need to sort chunks by distance from player so that transparency works properly
    glm::ivec2 playerWorldIdx = Chunk::getWorldIndex(player.position);
    std::sort(chunksSorted.begin(), chunksSorted.end(), [&](const Chunk* a, const Chunk* b) {
        glm::vec2 diffA = a->worldIndex - playerWorldIdx;
        glm::vec2 diffB = b->worldIndex - playerWorldIdx;
        return glm::dot(diffA, diffA) > glm::dot(diffB, diffB);
    });

    for (const Chunk* chunk : chunksSorted) {
        if (chunk->mesh) chunk->mesh->render(camera, aspectRatio);
    }
}

size_t World::getNumChunks() const {
    return chunks.size();
}

void World::update() {
    PROFILE_FUNCTION();

    player.update();

    if (player.movedChunks() || chunks.empty()) {
        glm::ivec2 worldIdx = Chunk::getWorldIndex(player.position);
        {
            PROFILE_SCOPE("Unload Chunks");

            for (auto it = chunks.begin(); it != chunks.end();) {
                auto diff = glm::abs(it->first - worldIdx);
                if (diff.x > renderDistance + 2 || diff.y > renderDistance + 2) {
                    it = chunks.erase(it);
                } else {
                    it++;
                }
            }
        }
        {
            PROFILE_SCOPE("Load Chunks");

            for (int z = worldIdx.y - renderDistance; z < worldIdx.y + renderDistance + 1; z++) {
                for (int x = worldIdx.x - renderDistance; x < worldIdx.x + renderDistance + 1;
                     x++) {
                    auto idx = glm::ivec2(x, z);
                    // Optimize: the unordered_maps are slow (kind of)
                    if (generateChunk(idx, true).second) {
                        // LOG("Generated chunk (" << x << ", " << z << ")");
                    }
                }
            }
        }
    }
}

std::pair<World::ChunkMapType::iterator, bool>
World::generateChunk(glm::ivec2 worldIdx, bool createMesh) {
    PROFILE_FUNCTION();

    auto result = chunks.emplace(std::make_pair(worldIdx, Chunk(worldIdx)));
    std::pair<ChunkMapType::iterator, bool> a = result;
    auto it = result.first;
    bool wasInserted = result.second;

    Chunk& chunk = it->second;

    if (!wasInserted) {
        // This line can cause random segfaults btw
        if (createMesh && !chunk.mesh) chunk.mesh = std::make_unique<ChunkMesh>(chunk, *this);
        return result;
    }

    auto center = glm::vec3(-5, 110, 27);
    float radius = 30;

    for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
            auto worldPos = Chunk::getWorldPosition(worldIdx, glm::ivec3(x, 0, z));

            float height = glm::perlin(glm::vec2(worldPos.x, worldPos.z) / 32.0f);
            height = utils::mapValue(height, -1.0f, 1.0f, 80.0f, 100.0f);

            int intHeight = height;

            for (int cy = 0; cy < Chunk::CHUNK_HEIGHT; cy++) {
                worldPos.y = cy;
                if (cy < intHeight) {
                    BlockId b = cy == intHeight - 1 ? Block::GRASS : Block::STONE;
                    chunk.setBlock({x, cy, z}, b);
                }
                float dist = glm::distance(center, (glm::vec3)worldPos);
                if (dist <= radius && dist >= radius - 1) {
                    chunk.setBlock({x, cy, z}, Block::GLASS);
                }
            }
        }
    }
    if (createMesh) chunk.mesh = std::make_unique<ChunkMesh>(chunk, *this);

    return result;
}
