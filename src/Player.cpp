#include "Player.hpp"

#include "world/Chunk.hpp"
#include "Application.hpp"

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

void Player::handleMovement(float deltaTime) {
    glm::vec3 camFront = camera.getDirection();
    camFront.y = 0;
    camFront = glm::normalize(camFront);

    glm::vec3 movementDirection = glm::vec3(0);

    GLFWwindow* window = Application::get().getWindow();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        movementDirection += glm::vec3(1) * camFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        movementDirection -= glm::vec3(1) * camFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        movementDirection -= glm::normalize(glm::cross(camFront, camera.getUp()));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        movementDirection += glm::normalize(glm::cross(camFront, camera.getUp()));
    }

    if (movementDirection != glm::vec3(0)) movementDirection = glm::normalize(movementDirection);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        movementDirection += glm::vec3(0, 1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        movementDirection -= glm::vec3(0, 1, 0);
    }

    position += movementDirection * movementSpeed * deltaTime;
}

void Player::updateCameraFromMouse(glm::ivec2 mouseOffset) {
    yaw += mouseOffset.x * camSensitivity;
    pitch += mouseOffset.y * camSensitivity;
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
    // std::cout << mouseOffset.y << " " << pitch << " " << yaw << "\n";
}

void Player::update(float deltaTime) {
    Application& app = Application::get();

    handleMovement(deltaTime);

    auto idx = Chunk::getWorldIndex(position);
    _movedChunks = false;
    if (prevWorldIdx != idx) _movedChunks = true;

    calculateCameraDirection();
    camera.position = position;

    // Interacting with the world
    utils::Ray ray;
    ray.origin = position;
    ray.direction = camera.direction;
    ray.length = 6;

    auto result = app.world->castRay(ray);
    if (result.hit) {
        // std::cout << result.blockPosition.x << ", " << result.blockPosition.y << ", "
        //           << result.blockPosition.z << "\n";
        if (app.getMouse().buttons[GLFW_MOUSE_BUTTON_LEFT].wasPressed)
            app.world->setBlock(result.blockPosition, Block::AIR);
    }

    prevWorldIdx = idx;
}

bool Player::movedChunks() const {
    return _movedChunks;
}

void Player::placeBlock() {
    // Application& app = Application::get();
    // World* world = app.world.get();
}
