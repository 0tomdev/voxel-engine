#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Chunk.hpp"
#include "ChunkMesh.hpp"
#include "Player.hpp"

class World {
public:
    std::unordered_map<glm::ivec2, Chunk> chunks;
    // You can't use operator[] bc there's no default constructor for Chunk

    int renderDistance = 6;

    Player player;

    World();

    BlockId getBlock(glm::ivec3 pos) const;
    void setBlock(glm::ivec3 pos, BlockId block);
    void render(const Camera& camera, float aspectRatio) const;
    size_t getNumChunks() const;
    void update();

private:
    // Generates terrain for a chunk (does not create mesh)
    bool generateChunk(glm::ivec2 worldIndex);
};