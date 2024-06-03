#include "Chunk.hpp"
#include "ChunkMesh.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

class World {
public:
    std::unordered_map<glm::ivec2, Chunk> chunks;
    /** Things are going to get messed up if the meshes are being copied around bc of the VBOs and
     * EBOs. Also you can't use operator[] bc there's no default constructor
     */
    std::unordered_map<glm::ivec2, ChunkMesh> chunkMeshes;

    World();

    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
    void render(const Camera& camera, float aspectRatio) const;
    size_t getNumChunks() const;

private:
    // Generates terrain for a chunk (does not create mesh)
    void generateChunk(glm::ivec2 worldIndex);
};