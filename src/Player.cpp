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

void Player::physicsUpdate(float deltaTime) {
    Application& app = Application::get();

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
        position.y += 6 * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position.y -= 6 * deltaTime;
    }

    velocity += movementDirection * deltaTime * movementSpeed;

    float xzMult = 0.96;
    velocity.x *= xzMult;
    velocity.z *= xzMult;

    position += velocity * deltaTime;

    // Collision checking
    // boundingBox.pos = position + glm::vec3(0.1, 0, 0.1);
    // glm::ivec3 offset;
    // for (offset.y = -2; offset.y <= 2; offset.y++) {
    //     for (offset.z = -2; offset.z <= 2; offset.z++) {
    //         for (offset.x = -2; offset.x <= 2; offset.x++) {
    //             glm::ivec3 voxelPos = offset + (glm::ivec3)glm::floor(boundingBox.pos);
    //             BlockId block = app.world->getBlock(voxelPos);
    //             if (!block) continue;
    //             BoundingBox voxelBox = {glm::vec3(1), voxelPos};
    //             if (boundingBox.isTouching(voxelBox)) {
    //                 std::cout << "Collision\n";
    //                 break;
    //             }
    //         }
    //     }
    // }
}

void Player::updateCameraFromMouse(glm::ivec2 mouseOffset) {
    yaw += mouseOffset.x * camSensitivity;
    pitch += mouseOffset.y * camSensitivity;
    pitch = glm::clamp(pitch, -89.9f, 89.9f);
}

void Player::update(float deltaTime) {
    Application& app = Application::get();

    // Physics
    physicsUpdate(deltaTime);

    // Check if moved chunks
    auto idx = Chunk::getWorldIndex(position);
    _movedChunks = false;
    if (prevWorldIdx != idx) _movedChunks = true;

    // Camera position
    calculateCameraDirection();
    camera.position = position;

    // Breaking/placing blocks
    utils::Ray ray;
    ray.origin = position;
    ray.direction = camera.direction;
    ray.length = 6;

    auto result = app.world->castRay(ray);
    if (result.hit) {
        selectedBlock = {result.blockPosition, result.face};
        if (app.getMouse().buttons[GLFW_MOUSE_BUTTON_RIGHT].wasPressed) {
            app.world->setBlock(
                result.blockPosition + utils::getDirectionOffset(result.face), Block::GLASS
            );
        }
        if (app.getMouse().buttons[GLFW_MOUSE_BUTTON_LEFT].wasPressed)
            app.world->setBlock(result.blockPosition, Block::AIR);

    } else {
        selectedBlock.reset();
    }

    prevWorldIdx = idx;
}

bool Player::movedChunks() const {
    return _movedChunks;
}

const std::optional<Player::SelectedBlock>& Player::getSelectedBlock() const {
    return selectedBlock;
}
