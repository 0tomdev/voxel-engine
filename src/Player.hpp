#include <glm/glm.hpp>

#include "Camera.hpp"

class Player {
public:
    glm::vec3 position = glm::vec3(0, 150, 0);
    Camera camera;

    void calculateCameraDirection();
    void updateCameraFromMouse(glm::ivec2 mouseOffset);

private:
    float pitch = -89;
    float yaw = 0;
    float camSensitivity = 0.1;
};