#include <glm/glm.hpp>

#include "misc.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

class Chunk {
public:
    static std::optional<Shader> shader;
    static unsigned int vertexColorLocation;
    static unsigned int modelLoc;
    static unsigned int viewLoc;
    static unsigned int projectionLoc;

    static void init();

    unsigned int VAO;
    BlockID data[CHUNK_SIZE] = { 0 };

    Chunk();

    void render(Camera camera);
};