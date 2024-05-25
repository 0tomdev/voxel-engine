#pragma once

#include <set>

#include "Chunk.hpp"

class ChunkMesh {
public:
    struct Vertex {
        float x, y, z;
        float u, v;
        // Normals can only have 6 possible states because we are using cubes
        unsigned int normal;
    };

    ChunkMesh(Chunk& chunk);

    void addQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
    void render() const;

private:
    std::vector<Vertex> triangleVerts; // Does not use EBO
    unsigned int VBO;

    // std::vector<Vertex> vertices;
    // std::vector<unsigned int> triangles;

    void addVertex(Vertex vert);
    void addTriangle(Vertex v1, Vertex v2, Vertex v3);
};