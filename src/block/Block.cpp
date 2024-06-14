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

    BlockData glass;
    glass.textureIdx = 12;
    glass.isTransparent = true;
    blockDefs[GLASS] = glass;

    BlockData log;
    log.textureIdx = 9;
    blockDefs[LOG] = log;

    BlockData leaves;
    leaves.textureIdx = 13;
    leaves.isTransparent = true;
    blockDefs[LEAVES] = leaves;

    BlockData clay;
    clay.textureIdx = 8;
    blockDefs[CLAY] = clay;

    BlockData sand;
    sand.textureIdx = 6;
    blockDefs[SAND] = sand;

    BlockData water;
    water.textureIdx = 16;
    water.isTransparent = true;
    blockDefs[WATER] = water;
}

bool Block::isTransparent(BlockId block) {
    return blockDefs[block].isTransparent;
}

bool Block::isAirOrTransparent(BlockId block) {
    return !block || blockDefs[block].isTransparent;
}

static void registerBlock(Block::Name blockId, Block::BlockData) {}
