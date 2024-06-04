#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Chunk.hpp"

struct Camera {
public:
    const glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 position = glm::vec3(6, Chunk::CHUNK_HEIGHT, 6);
    glm::vec3 direction;

    float fov = 60.0f;
    float nearClip = 0.1f;
    float farClip = 256.0f;

    Camera();

    const glm::vec3& getDirection();
    const glm::vec3& getUp();

    glm::mat4 getViewMatrix() const;
};