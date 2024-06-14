#include <glm/gtc/noise.hpp>

#include "WorldGenerator.hpp"

void WorldGenerator::generateTerrain(Chunk& chunk) {
    const int dirtLayerHeight = 2;
    const int waterLevel = 90;

    for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
        for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
            auto worldPos = Chunk::getWorldPosition(chunk.worldIndex, glm::ivec3(x, 0, z));

            float num = utils::getRandom(0.0f, 1.0f);

            float mult = 0.5;
            float amplMult = 1.4;
            // float height = glm::simplex(glm::vec2(worldPos.x, worldPos.z) / 128.0f);
            float height = utils::octiveNoise(glm::vec2(worldPos.x, worldPos.z) / 200.0f, 4);
            height = height * 28.0f + 100.0f;

            int intHeight = (int)height;

            for (int y = 0; y < std::max(intHeight, waterLevel); y++) {
                worldPos.y = y;

                BlockId b;
                if (y < intHeight) {
                    if (y == intHeight - 1) {
                        b = Block::GRASS;
                    } else if (y >= intHeight - (1 + dirtLayerHeight)) {
                        b = Block::DIRT;
                    } else {
                        b = Block::STONE;
                    }
                } else {
                    b = Block::GLASS;
                }
                chunk.setBlock({x, y, z}, b);
            }

            // Trees!
            if (num < 0.005f) {
                const int treeHeight = 4;
                for (int i = 0; i < treeHeight + 1; i++) {
                    chunk.setBlock({x, intHeight + i, z}, Block::LOG);
                }

                glm::ivec3 offset;
                for (offset.y = -2; offset.y <= 1; offset.y++) {
                    for (offset.z = -2; offset.z <= 2; offset.z++) {
                        for (offset.x = -2; offset.x <= 2; offset.x++) {
                            glm::ivec3 pos = glm::ivec3(x, intHeight + treeHeight, z) + offset;
                            if (!Chunk::inBounds(pos)) continue;
                            if (chunk.getBlock(pos)) continue;

                            if (offset.y >= 0 && abs(offset.x) + abs(offset.z) > 1) continue;
                            if (offset.y == -1 && abs(offset.x) + abs(offset.z) == 4) continue;

                            chunk.setBlock(pos, Block::CLAY);
                        }
                    }
                }
            }
        }
    }
}