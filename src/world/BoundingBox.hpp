#include <glm/glm.hpp>

struct BoundingBox {
    glm::vec3 size;
    glm::vec3 pos;

    bool isTouching(const BoundingBox& other);
};