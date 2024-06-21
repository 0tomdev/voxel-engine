#pragma once

#include <glm/glm.hpp>

#include "utils.hpp"
#include "Camera.hpp"

class Player {
    struct SelectedBlock {
        glm::ivec3 position;
        utils::Direction face;
    };

public:
    glm::vec3 position = glm::vec3(0, 150, 0);
    Camera camera;
    float movementSpeed = 15;

    Player();
    void updateCameraFromMouse(glm::ivec2 mouseOffset);
    void update(float deltaTime);
    // Returns true if the player moved into a new chunk this frame
    bool movedChunks() const;
    const std::optional<SelectedBlock>& getSelectedBlock() const;

private:
    float pitch = -30;
    float yaw = 0;
    float camSensitivity = 0.1;
    glm::ivec2 prevWorldIdx;
    bool _movedChunks = false;
    std::optional<SelectedBlock> selectedBlock;

    void calculateCameraDirection();
    void handleMovement(float deltaTime);
};