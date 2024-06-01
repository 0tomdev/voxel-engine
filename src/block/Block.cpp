#include "Block.hpp"

int Block::BlockData::getTextureIdx(utils::Direction dir) const {
    if (dir == utils::UP) {
        return topTextureIdx.value_or(textureIdx);
    }
    if (dir == utils::DOWN) {
        return bottomTextureIdx.value_or(textureIdx);
    }
    return sideTextureIdx.value_or(textureIdx);
}

Block::BlockData::BlockData() {}

void Block::initBlocks() {
    BlockData stone;
    stone.textureIdx = 0;
    blockDefs[STONE] = stone;

    BlockData stoneBricks;
    stoneBricks.textureIdx = 1;
    blockDefs[STONE_BRICKS] = stoneBricks;

    BlockData dirt;
    dirt.textureIdx = 2;
    blockDefs[DIRT] = dirt;

    BlockData grass;
    grass.bottomTextureIdx = 2;
    grass.topTextureIdx = 3;
    grass.sideTextureIdx = 4;
    blockDefs[GRASS] = grass;
}

static void registerBlock(Block::Name blockId, Block::BlockData) {}
