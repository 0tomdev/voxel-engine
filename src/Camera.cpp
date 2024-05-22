#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera.hpp"

Camera::Camera() {}

glm::mat4 Camera::getViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(position, position + direction, up);

    return view;
}

void Camera::calculateDirection() {
    // https://stackoverflow.com/questions/30011741/3d-vector-defined-by-2-angles
    float hAnngle = glm::radians(yaw);
    float vAngle = glm::radians(pitch);
    direction.x = cos(hAnngle) * cos(vAngle);
    direction.z = sin(hAnngle) * cos(vAngle);
    direction.y = sin(vAngle);

    direction = glm::normalize(direction);

    // Positive z is towards the screen
    // Positive y is up
    // Positive x is right
    // std::cout << "Position:" << position.x << ", " << position.y << ", " << position.z
    //           << "\n";
}

void Camera::updateFromMouse(const glm::vec2& mouseOffset) {
    yaw += mouseOffset.x * sensitivity;
    pitch += mouseOffset.y * sensitivity;
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
    // std::cout << mouseOffset.y << " " << pitch << " " << yaw << "\n";
}

const glm::vec3& Camera::getDirection() {
    return direction;
}

const glm::vec3& Camera::getUp() {
    return up;
}
