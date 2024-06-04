#include "Player.hpp"

void Player::calculateCameraDirection() {
    // https://stackoverflow.com/questions/30011741/3d-vector-defined-by-2-angles
    float hAnngle = glm::radians(yaw);
    float vAngle = glm::radians(pitch);
    camera.direction.x = cos(hAnngle) * cos(vAngle);
    camera.direction.z = sin(hAnngle) * cos(vAngle);
    camera.direction.y = sin(vAngle);

    camera.direction = glm::normalize(camera.direction);
    camera.position = position;
}

void Player::updateCameraFromMouse(glm::ivec2 mouseOffset) {
    yaw += mouseOffset.x * camSensitivity;
    pitch += mouseOffset.y * camSensitivity;
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
    // std::cout << mouseOffset.y << " " << pitch << " " << yaw << "\n";
}
