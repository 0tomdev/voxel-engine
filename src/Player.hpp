#pragma once

#include <glm/glm.hpp>

#include "Camera.hpp"

class Player {
public:
    glm::vec3 position = glm::vec3(0, 150, 0);
    Camera camera;
    float movementSpeed = 15;

    Player();
    void updateCameraFromMouse(glm::ivec2 mouseOffset);
    void update(float deltaTime);
    // Returns true if the player moved into a new chunk this frame
    bool movedChunks() const;
    void placeBlock();
    // void breakBlock();

private:
    float pitch = -30;
    float yaw = 0;
    float camSensitivity = 0.1;
    glm::ivec2 prevWorldIdx;
    bool _movedChunks = false;

    void calculateCameraDirection();
    void handleMovement(float deltaTime);
};