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

const glm::vec3& Camera::getDirection() const {
    return direction;
}

const glm::vec3& Camera::getUp() const {
    return up;
}
