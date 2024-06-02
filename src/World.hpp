#include "Chunk.hpp"
#include "ChunkMesh.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

class World {
public:
    World();

    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
    void render(const Camera& camera, float aspectRatio) const;

private:
    std::unordered_map<glm::ivec2, Chunk> chunks;
    // Things are going to get messed up if the meshes are being copied around bc of the
    // VBOs and EBOs
    std::unordered_map<glm::ivec2, ChunkMesh> chunkMeshes;
};