#pragma once

#include "Chunk.hpp"
#include "Camera.hpp"

class Chunk;
class World;

class ChunkMesh {
public:
    struct Vertex {
        glm::vec3 pos;
        float u, v;
        unsigned int normal;
        unsigned int aoValue = 3;
        int textureIdx;
        GLubyte isLowered = 0; // makes water a bit shorter than regular blocks

        Vertex(float x, float y, float z, float u, float v, unsigned int normal)
            : pos(x, y, z), u(u), v(v), normal(normal) {}

        uint32_t pack() const;
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        GLuint VAO;
        GLuint VBO;

        void createBuffers();
        void deleteBuffers();
        void render() const;
    };

    enum MeshType { OPAQUE, TRANSPARENT };

    /**
     * Bordering chunks layout
     *
     * 0 1 2
     * 3 4 5
     * 6 7 8
     *
     * 4 is the chunk that the mesh is for
     */

    class BorderingChunks {
    private:
        // This code is fucked
        // These will be dangling pointers when the neighboring chunks get unloaded! 😱
        std::vector<const Chunk*> chunks;

    public:
        BorderingChunks(const Chunk& middleChunk, World& world);
        BlockId getBlock(glm::ivec3 chunkPos) const;
        const Chunk* const getMiddle(); // double const pointer :O
    };

    static const size_t vertexSize = sizeof(Vertex);
    uint32_t generationTime;

    ChunkMesh(const Chunk& chunk, World& world);
    ~ChunkMesh();

    static void init();

    void render(const Camera& camera, float aspectRatio) const;
    size_t getSize() const;

private:
    const Chunk& chunk;
    BorderingChunks nearChunks;

    Mesh opaqueMesh;
    Mesh transparentMesh;

    void createMesh(World& world);
    void addFace(const glm::vec3& pos, utils::Direction facing);
    void addQuad(const glm::vec3& pos, int facing, int textureIdx, Mesh& mesh, bool isLiquid);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3, Mesh& mesh);
    void
    calculateAO(std::vector<Vertex*>& verts, utils::Direction facing, const glm::ivec3& pos) const;
    bool shouldAddFace(BlockId thisBlock, BlockId otherBlock) const;
};