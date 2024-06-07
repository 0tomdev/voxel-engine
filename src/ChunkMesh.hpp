#pragma once

#include "Chunk.hpp"
#include "Camera.hpp"

class Chunk;

class ChunkMesh {
public:
    struct Vertex {
        // 3x int 0-16 (5 bits)
        glm::vec3 pos;

        // 2x int 0-1 (1 bit)
        float u, v;

        // int 0-5 (3 bits)
        unsigned int normal;

        // int 0-255 (8 bits)
        int textureIdx;

        // Total size: 28 bits packed inside 32 bit integer

        Vertex(float x, float y, float z, float u, float v, unsigned int normal)
            : pos(x, y, z), u(u), v(v), normal(normal) {}

        uint32_t pack() const;
    };

    static const size_t vertexSize = 5 * sizeof(float) + 2 * sizeof(unsigned int);

    ChunkMesh(const Chunk& chunk);
    ~ChunkMesh();

    static void init();

    void render(const Camera& camera, float aspectRatio, glm::ivec2 worldIndex) const;
    size_t getSize() const;
    void deleteBuffers(); // Can't be put in destructor :(

private:
    std::vector<Vertex> triangleVerts; // Does not use EBO
    GLuint VAO;
    GLuint VBO;

    void createMeshBetter(const Chunk& chunk);

    void addFace(const glm::vec3& pos, utils::Direction facing, const Chunk& chunk);
    void addQuad(const glm::vec3& pos, int facing, int textureIdx);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3);
};