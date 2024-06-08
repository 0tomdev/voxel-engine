#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Chunk.hpp"
#include "ChunkMesh.hpp"
#include "Player.hpp"

class World {
public:
    using ChunkMapType = std::unordered_map<glm::ivec2, Chunk>;
    ChunkMapType chunks;

    int renderDistance = 6;

    Player player;

    World();

    BlockId getBlock(glm::ivec3 pos) const;
    BlockId getBlockOrGenChunk(glm::ivec3 pos);
    void setBlock(glm::ivec3 pos, BlockId block);
    void render(const Camera& camera, float aspectRatio) const;
    size_t getNumChunks() const;
    void update();

    // private:
    // Generates terrain for a chunk (does not create mesh)
    std::pair<World::ChunkMapType::iterator, bool>
    generateChunk(glm::ivec2 worldIdx, bool createMesh);
};