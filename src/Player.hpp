#include <glm/glm.hpp>

#include "Camera.hpp"

class Player {
public:
    glm::vec3 position = glm::vec3(0, 150, 0);
    Camera camera;

    Player();
    void updateCameraFromMouse(glm::ivec2 mouseOffset);
    void update();
    // Returns true if the player moved into a new chunk this frame
    bool movedChunks() const;

private:
    float pitch = -89;
    float yaw = 0;
    float camSensitivity = 0.1;
    glm::ivec2 prevWorldIdx;
    bool _movedChunks = false;

    void calculateCameraDirection();
};