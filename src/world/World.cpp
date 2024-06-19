#include "World.hpp"

#include "../Application.hpp"

World::World() {}

BlockId World::getBlock(glm::ivec3 worldPos) const {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    auto it = chunks.find(chunkWorldIdx);
    if (it == chunks.end()) return Block::AIR;

    const Chunk& c = it->second;
    return c.getBlock(c.getChunkPosition(worldPos));
}

BlockId World::getBlockOrGenChunk(glm::ivec3 worldPos) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    auto result = generateChunk(worldPos, false);
    return result.first->second.getBlock(worldPos & Chunk::CHUNK_SIZE);
}

void World::setBlock(glm::ivec3 worldPos, BlockId block) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    Chunk& c = chunks.at(chunkWorldIdx);
    c.setBlock(c.getChunkPosition(worldPos), block);
    if (c.mesh) {
        c.mesh->shouldRebuild = true;
    }
}

void World::render(float aspectRatio) const {
    auto& app = Application::get();

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

    glBindTexture(GL_TEXTURE_2D, app.textures.at("texture_atlas").id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glCullFace(GL_BACK);
    for (const Chunk* chunk : chunksSorted) {
        if (chunk->mesh) chunk->mesh->render(player.camera, aspectRatio);
    }
}

size_t World::getNumChunks() const {
    return chunks.size();
}

void World::update(float deltaTime) {
    player.update(deltaTime);

    if (player.movedChunks() || chunks.empty()) {
        glm::ivec2 worldIdx = Chunk::getWorldIndex(player.position);
        for (auto it = chunks.begin(); it != chunks.end();) {
            auto diff = glm::abs(it->first - worldIdx);
            if (diff.x > renderDistance + 2 || diff.y > renderDistance + 2) {
                it = chunks.erase(it);
            } else {
                it++;
            }
        }

        for (int z = worldIdx.y - renderDistance; z < worldIdx.y + renderDistance + 1; z++) {
            for (int x = worldIdx.x - renderDistance; x < worldIdx.x + renderDistance + 1; x++) {
                auto idx = glm::ivec2(x, z);
                if (generateChunk(idx, true).second) {
                    // LOG("Generated chunk (" << x << ", " << z << ")");
                }
            }
        }
    }
}

std::pair<World::ChunkMapType::iterator, bool>
World::generateChunk(glm::ivec2 worldIdx, bool createMesh) {
    auto result = chunks.emplace(std::make_pair(worldIdx, Chunk(worldIdx)));
    std::pair<ChunkMapType::iterator, bool> a = result;
    auto it = result.first;
    bool wasInserted = result.second;

    Chunk& chunk = it->second;

    if (!wasInserted) {
        // This line can cause random segfaults btw
        if (createMesh && !chunk.mesh) {
            chunk.mesh = std::make_unique<ChunkMesh>(chunk);
        }
        return result;
    }

    // gen terrain
    worldGen.generateTerrain(chunk);

    if (createMesh) chunk.mesh = std::make_unique<ChunkMesh>(chunk);

    return result;
}

World::RayCastResult World::castRay(const utils::Ray& ray) {
    using Direction = utils::Direction;

    RayCastResult result;
    glm::vec3 currentPos = ray.origin;

    glm::vec3 step = glm::sign(ray.direction);
    glm::vec3 deltaT = glm::abs(1.0f / ray.direction);

    // Initialize t variables
    glm::vec3 nextT = deltaT * (1.0f - glm::fract(currentPos));

    for (float t = 0.0f; t < ray.length;) {
        glm::ivec3 blockPos = glm::floor(currentPos);
        // std::cout << "  " << blockPos.x << ", " << blockPos.y << ", " << blockPos.z << "\n";

        if (getBlock(blockPos)) {
            result.hit = true;
            result.blockPosition = blockPos;

            if (step.x < 0.0f) {
                result.face = Direction::EAST;
            } else if (step.x > 0.0f) {
                result.face = Direction::WEST;
            } else if (step.y < 0.0f) {
                result.face = Direction::UP;
            } else if (step.y > 0.0f) {
                result.face = Direction::DOWN;
            } else if (step.z < 0.0f) {
                result.face = Direction::SOUTH;
            } else if (step.z > 0.0f) {
                result.face = Direction::NORTH;
            }

            return result;
        }

        // Find the smallest t value to advance to the next voxel
        if (nextT.x < nextT.y && nextT.x < nextT.z) {
            currentPos.x += step.x;
            t = nextT.x;
            nextT.x += deltaT.x;
        } else if (nextT.y < nextT.z) {
            currentPos.y += step.y;
            t = nextT.y;
            nextT.y += deltaT.y;
        } else {
            currentPos.z += step.z;
            t = nextT.z;
            nextT.z += deltaT.z;
        }
    }

    result.hit = false;
    return result;
}
