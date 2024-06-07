#include "Player.hpp"
#include "Chunk.hpp"

Player::Player() : prevWorldIdx(Chunk::getWorldIndex(position)) {}

void Player::calculateCameraDirection() {
    // https://stackoverflow.com/questions/30011741/3d-vector-defined-by-2-angles
    float hAnngle = glm::radians(yaw);
    float vAngle = glm::radians(pitch);
    camera.direction.x = cos(hAnngle) * cos(vAngle);
    camera.direction.z = sin(hAnngle) * cos(vAngle);
    camera.direction.y = sin(vAngle);

    camera.direction = glm::normalize(camera.direction);
}

void Player::updateCameraFromMouse(glm::ivec2 mouseOffset) {
    yaw += mouseOffset.x * camSensitivity;
    pitch += mouseOffset.y * camSensitivity;
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
    // std::cout << mouseOffset.y << " " << pitch << " " << yaw << "\n";
}

void Player::update() {
    auto idx = Chunk::getWorldIndex(position);
    _movedChunks = false;
    if (prevWorldIdx != idx) _movedChunks = true;

    calculateCameraDirection();
    camera.position = position;

    prevWorldIdx = idx;
}

bool Player::movedChunks() const {
    return _movedChunks;
}
