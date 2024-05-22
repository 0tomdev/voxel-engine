#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Camera {
private:
    float pitch = 0;
    float yaw = -90;
    float sensitivity = 0.1;
    const glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 direction;

public:
    glm::vec3 position = glm::vec3(0, 0, 3);

public:
    Camera();

    const glm::vec3& getDirection();
    const glm::vec3& getUp();

    glm::mat4 getViewMatrix() const;
    void updateFromMouse(const glm::vec2& mouseOffset);
    void calculateDirection();
};