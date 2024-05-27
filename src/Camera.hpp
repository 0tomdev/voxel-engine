#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"

class Camera {
public:
    glm::vec3 position = glm::vec3(-6, Chunk::CHUNK_HEIGHT, -6);

    Camera();

    const glm::vec3& getDirection();
    const glm::vec3& getUp();

    glm::mat4 getViewMatrix() const;
    void updateFromMouse(const glm::vec2& mouseOffset);
    void calculateDirection();

private:
    float pitch = 0;
    float yaw = 0;
    float sensitivity = 0.1;
    const glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 direction;
};