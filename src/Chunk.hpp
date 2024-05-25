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

    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 32;
    static const int CHUNK_ARRAY_SIZE = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    static std::optional<Shader> shader;
    static unsigned int vertexColorLocation;
    static unsigned int modelLoc;
    static unsigned int viewLoc;
    static unsigned int projectionLoc;
    static std::vector<Vertex> allCubeVertices;

    static void init();

    unsigned int VAO;
    BlockID* data;

    Chunk();
    ~Chunk();

    void render(Camera camera);
};