#include "ChunkMesh.hpp"
#include "utils.hpp"

ChunkMesh::ChunkMesh(Chunk& chunk) {
    utils::ScopeTimer timer;
    // The rest
}

void ChunkMesh::addQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4) {
    addTriangle(v1, v2, v3);
    addTriangle(v2, v2, v4);
}

void ChunkMesh::addVertex(Vertex vert) {}

void ChunkMesh::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    triangleVerts.push_back(v1);
    triangleVerts.push_back(v2);
    triangleVerts.push_back(v3);
}

void ChunkMesh::render() const {}