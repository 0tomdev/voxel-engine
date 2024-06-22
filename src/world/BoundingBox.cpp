#include "BoundingBox.hpp"

bool BoundingBox::isTouching(const BoundingBox& other) {
    // Check if there is any overlap in the x, y, and z dimensions
    bool overlapX = pos.x < other.pos.x + other.size.x && pos.x + size.x > other.pos.x;
    bool overlapY = pos.y < other.pos.y + other.size.y && pos.y + size.y > other.pos.y;
    bool overlapZ = pos.z < other.pos.z + other.size.z && pos.z + size.z > other.pos.z;

    // If there is overlap in all three dimensions, the boxes are touching
    return overlapX && overlapY && overlapZ;
}