#include <glm/gtc/noise.hpp>

#include "WorldGenerator.hpp"

void WorldGenerator::generateTerrain(Chunk& chunk) {
    for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
            auto worldPos = Chunk::getWorldPosition(chunk.worldIndex, glm::ivec3(x, 0, z));

            float num = utils::getRandom(0.0f, 1.0f);

            float height = glm::simplex(glm::vec2(worldPos.x, worldPos.z) / 128.0f);
            height += 2 * glm::simplex(glm::vec2(worldPos.x, worldPos.z) / 64.0f);
            height = utils::mapValue(height, -1.0f, 1.0f, 70.0f, 90.0f);

            int intHeight = (int)height;

            for (int cy = 0; cy < intHeight; cy++) {
                worldPos.y = cy;
                const int dirtLayerHeight = 2;
                if (cy < intHeight) {
                    BlockId b;
                    if (cy == intHeight - 1) {
                        b = Block::GRASS;
                    } else if (cy >= intHeight - (1 + dirtLayerHeight)) {
                        b = Block::DIRT;
                    } else {
                        b = Block::STONE;
                    }
                    chunk.setBlock({x, cy, z}, b);
                }
            }

            // Trees!
            if (num < 0.005f) {
                const int treeHeight = 4;
                for (int i = 0; i < treeHeight + 1; i++) {
                    chunk.setBlock({x, intHeight + i, z}, Block::LOG);
                }

                glm::ivec3 offset;
                for (offset.y = -2; offset.y <= 0; offset.y++) {
                    for (offset.z = 0; offset.z <= 2; offset.z++) {
                        for (offset.x = -2; offset.x <= 2; offset.x++) {
                            glm::ivec3 pos = glm::ivec3(x, intHeight + treeHeight, z) + offset;
                            if (!Chunk::inBounds(pos)) continue;
                            if (chunk.getBlock(pos)) continue;

                            if (offset.y >= 0 && abs(offset.x) + abs(offset.z) != 1) continue;
                            if (offset.y == -1 && abs(offset.x) + abs(offset.z) == 4) continue;

                            chunk.setBlock(pos, Block::CLAY);
                        }
                    }
                }
            }
        }
    }
}