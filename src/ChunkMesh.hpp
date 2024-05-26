#pragma once

#include "Chunk.hpp"

class ChunkMesh {
public:
    struct Vertex {
        glm::vec3 pos;
        float u, v;
        // Normals can only have 6 possible states because we are using cubes
        unsigned int normal;

        Vertex(float x, float y, float z, float u, float v, unsigned int normal)
            : pos(x, y, z), u(u), v(v), normal(normal) {}
    };

    ChunkMesh(Chunk& chunk);

    void render(Camera& camera) const;

private:
    std::vector<Vertex> triangleVerts; // Does not use EBO
    GLuint VAO;
    GLuint VBO;

    // std::vector<Vertex> vertices;
    // std::vector<unsigned int> triangles;

    void addQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
    void addVertex(Vertex vert);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3);
};