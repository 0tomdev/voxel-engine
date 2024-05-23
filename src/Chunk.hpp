#include <glm/glm.hpp>

#include "misc.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

class Chunk {
public:
    struct Vertex {
        float x, y, z;
        float u, v;
        // Normals can only have 6 possible states because we are using cubes
        unsigned int normal;
    };
    enum Direction { EAST, WEST, UP, DOWN, SOUTH, NORTH };

    static std::optional<Shader> shader;
    static unsigned int vertexColorLocation;
    static unsigned int modelLoc;
    static unsigned int viewLoc;
    static unsigned int projectionLoc;
    static std::vector<Vertex> allCubeVertices;

    static void init();

    unsigned int VAO;
    BlockID data[CHUNK_SIZE] = { 0 };

    Chunk();

    void render(Camera camera);
};