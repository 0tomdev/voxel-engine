#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ChunkMesh.hpp"
#include "utils.hpp"
#include "global.hpp"

static const size_t vertexSize = 5 * sizeof(float) + sizeof(unsigned int);

// clang-format off
static std::vector<ChunkMesh::Vertex> allCubeVertices = {
    // +x
    { 1, 1, 0, 1, 1, 0 }, { 1, 1, 1, 0, 1, 0 }, { 1, 0, 0, 1, 0, 0 }, { 1, 0, 1, 0, 0, 0 },

    // -x
    { 0, 1, 1, 1, 1, 1 }, { 0, 1, 0, 0, 1, 1 }, { 0, 0, 0, 0, 0, 1 }, { 0, 0, 1, 1, 0, 1 },

    // +y
    { 1, 1, 0, 1, 1, 2 }, { 0, 1, 0, 0, 1, 2 }, { 0, 1, 1, 0, 0, 2 }, { 1, 1, 1, 1, 0, 2 },

    // -y
    { 0, 0, 0, 0, 0, 3 }, { 1, 0, 0, 1, 0, 3 }, { 0, 0, 1, 0, 1, 3 }, { 1, 0, 1, 1, 1, 3 },

    // +z
    { 1, 1, 1, 1, 1, 4 }, { 0, 1, 1, 0, 1, 4 }, { 0, 0, 1, 0, 0, 4 }, { 1, 0, 1, 1, 0, 4 },

    // -z
    { 0, 1, 0, 0, 1, 5 }, { 1, 1, 0, 1, 1, 5 }, { 0, 0, 0, 0, 0, 5 }, { 1, 0, 0, 1, 0, 5 },
};
// clang-format on

ChunkMesh::ChunkMesh(Chunk& chunk) {
    assert(vertexSize == sizeof(Vertex));
    utils::ScopeTimer timer;
    // The rest
    int i = 0;
    for (int y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
            for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
                Vertex v1 = allCubeVertices[8];
                Vertex v2 = allCubeVertices[9];
                Vertex v3 = allCubeVertices[10];
                Vertex v4 = allCubeVertices[11];
                v1.pos += glm::vec3(x, y, z);
                v2.pos += glm::vec3(x, y, z);
                v3.pos += glm::vec3(x, y, z);
                v4.pos += glm::vec3(x, y, z);

                if (chunk.data[i]) {
                    addQuad(v1, v2, v3, v4);
                }
                i++;
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    std::cout << "Mesh vbo: " << VBO << "\n";
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, triangleVerts.size() * vertexSize, &triangleVerts[0],
        GL_STATIC_DRAW
    );

    // xyz coords
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glEnableVertexAttribArray(0);
    // uv coords
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, u))
    );
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2, 1, GL_UNSIGNED_INT, GL_FALSE, vertexSize, (void*)(offsetof(Vertex, normal))
    );
    glEnableVertexAttribArray(2);

    for (auto& v : triangleVerts) {
        std::cout << v.pos.x << ", " << v.pos.y << ", " << v.pos.z << "\n";
    }
}

void ChunkMesh::addQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4) {
    addTriangle(v1, v2, v3);
    addTriangle(v3, v1, v4);
}

void ChunkMesh::addVertex(Vertex vert) {}

void ChunkMesh::addTriangle(Vertex v1, Vertex v2, Vertex v3) {
    triangleVerts.push_back(v1);
    triangleVerts.push_back(v2);
    triangleVerts.push_back(v3);
}

void ChunkMesh::render(Camera& camera) const {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), Global::screenWidth / (float)Global::screenHeight, 0.1f,
        100.0f
    );
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0.01, 0));

    // Shader
    const Shader& shaderValue = Chunk::shader.value();
    GL_CALL(glUseProgram(shaderValue.ID));
    GL_CALL(glUniformMatrix4fv(Chunk::viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
    GL_CALL(
        glUniformMatrix4fv(Chunk::projectionLoc, 1, GL_FALSE, glm::value_ptr(projection))
    );
    GL_CALL(glUniformMatrix4fv(Chunk::modelLoc, 1, GL_FALSE, glm::value_ptr(model)));
    GL_CALL(glUniform1f(Chunk::opacityLoc, 1.0f));

    // Render
    GL_CALL(glBindVertexArray(VAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, triangleVerts.size()));
}