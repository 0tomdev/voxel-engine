#pragma once

#include "Chunk.hpp"
#include "Camera.hpp"

class ChunkMesh {
public:
    static const size_t vertexSize = 5 * sizeof(float) + 2 * sizeof(unsigned int);

    struct Vertex {
        glm::vec3 pos;
        float u, v;
        // Normals can only have 6 possible states because we are using cubes
        unsigned int normal;
        int textureIdx;

        Vertex(float x, float y, float z, float u, float v, unsigned int normal)
            : pos(x, y, z), u(u), v(v), normal(normal) {}
    };

    ChunkMesh(const Chunk& chunk);

    void render(const Camera& camera, float aspectRatio, glm::ivec2 worldIndex) const;
    size_t getSize() const;

private:
    std::vector<Vertex> triangleVerts; // Does not use EBO
    GLuint VAO;
    GLuint VBO;

    void addQuad(const glm::vec3& pos, int facing, int textureIdx);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3);
};