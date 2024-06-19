#include <glm/glm.hpp>

#include "utils.hpp"

namespace UI {
class Rect {
public:
    Rect();
    ~Rect();

    void render(const utils::Texture& texture) const;

private:
    glm::ivec2 position;
    glm::ivec2 size;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    void createBuffers();
    void deleteBuffers();
};
}