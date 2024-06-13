#pragma once

#include "Chunk.hpp"
#include "Camera.hpp"

class Chunk;
class World;

class ChunkMesh {
public:
    struct Vertex {
        // 3x int 0-16 (5 bits)
        glm::vec3 pos;

        // 2x int 0-1 (1 bit)
        float u, v;

        // int 0-5 (3 bits)
        unsigned int normal;

        // 0-3 (0 is most dark and 3 is least dark)
        unsigned int aoValue = 3;

        // int 0-255 (8 bits)
        int textureIdx;

        // Total size: 28 bits packed inside 32 bit integer

        Vertex(float x, float y, float z, float u, float v, unsigned int normal)
            : pos(x, y, z), u(u), v(v), normal(normal) {}

        uint32_t pack() const;
    };

    static const size_t vertexSize = 5 * sizeof(float) + 3 * sizeof(unsigned int);
    uint32_t generationTime;

    ChunkMesh(const Chunk& chunk, World& world);
    ~ChunkMesh();

    static void init();

    void render(const Camera& camera, float aspectRatio) const;
    size_t getSize() const;

private:
    std::vector<Vertex> triangleVerts; // Does not use EBO
    GLuint VAO;
    GLuint VBO;
    const Chunk& chunk;

    void createMeshBetter(World& world);
    void addFace(const glm::vec3& pos, utils::Direction facing);
    void addQuad(const glm::vec3& pos, int facing, int textureIdx);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3);
    void
    calculateAO(std::vector<Vertex*>& verts, utils::Direction facing, const glm::ivec3& pos) const;
};