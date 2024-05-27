#pragma once

#include <array>
#include <string>

#include "types.hpp"

struct BlockDef {
    std::string name;
    int textureIdx;
};

const int numBlocks = 1 << (sizeof(BlockID) * 8);

inline std::array<BlockDef, numBlocks> blockDefs = {
    BlockDef{"Air", -1}, BlockDef{"Stone", 0}, BlockDef{"Stone Bricks", 1},
    BlockDef{"Dirt", 2}
};