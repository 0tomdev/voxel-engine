#include "World.hpp"

#include "../imgui/imgui.h"

#include "../Application.hpp"

World::World() {}

BlockId World::getBlock(glm::ivec3 worldPos) const {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    auto it = chunks.find(chunkWorldIdx);
    if (it == chunks.end()) return Block::AIR;

    const Chunk& c = it->second;
    return c.getBlock(c.getChunkPosition(worldPos));
}

BlockId World::getBlockOrGenChunk(glm::ivec3 worldPos) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    auto result = generateChunk(worldPos, false);
    return result.first->second.getBlock(worldPos & Chunk::CHUNK_SIZE);
}

void World::setBlock(glm::ivec3 worldPos, BlockId block) {
    glm::ivec2 chunkWorldIdx = Chunk::getWorldIndex(worldPos);
    Chunk& c = chunks.at(chunkWorldIdx);
    c.setBlock(c.getChunkPosition(worldPos), block);
    if (c.mesh) {
        c.mesh->shouldRebuild = true;
    }
}

void World::render(float aspectRatio) const {
    auto& app = Application::get();

    std::vector<const Chunk*> chunksSorted(chunks.size());

    int i = 0;
    for (auto it = chunks.begin(); it != chunks.end(); it++) {
        chunksSorted[i] = &(it->second);
        i++;
    }

    // Need to sort chunks by distance from player so that transparency works properly
    glm::ivec2 playerWorldIdx = Chunk::getWorldIndex(player.position);
    std::sort(chunksSorted.begin(), chunksSorted.end(), [&](const Chunk* a, const Chunk* b) {
        glm::vec2 diffA = a->worldIndex - playerWorldIdx;
        glm::vec2 diffB = b->worldIndex - playerWorldIdx;
        return glm::dot(diffA, diffA) > glm::dot(diffB, diffB);
    });

    glBindTexture(GL_TEXTURE_2D, app.textures.at("texture_atlas").id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glCullFace(GL_BACK);
    for (const Chunk* chunk : chunksSorted) {
        if (chunk->mesh) chunk->mesh->render(player.camera, aspectRatio);
    }

    // TEMPORARY DEBUGGING CODE
    // glm::mat4 view = player.camera.getViewMatrix();
    // glm::mat4 projection = glm::perspective(
    //     glm::radians(player.camera.fov), aspectRatio, player.camera.nearClip,
    //     player.camera.farClip
    // );
    // glm::vec4 p1 = projection * view *
    //                glm::vec4(player.position + glm::normalize(player.camera.direction) * 0.1f,
    //                1);
    // glm::vec4 p2 =
    //     projection * view *
    //     glm::vec4(
    //         player.position + glm::normalize(player.camera.direction) * 6.0f + glm::vec3(2, 0,
    //         0), 1
    //     );
    // glm::vec3 ndc1 = p1 / p1.w;
    // glm::vec3 ndc2 = p2 / p2.w;

    // auto ss = app.getWindowSize();
    // glm::ivec2 start = glm::vec2((ndc1.x + 1.0f) * 0.5f * ss.x, (1.0f - ndc1.y) * 0.5f * ss.y);
    // glm::ivec2 end = glm::vec2((ndc2.x + 1.0f) * 0.5f * ss.x, (1.0f - ndc2.y) * 0.5f * ss.y);

    // ImDrawList* drawList = ImGui::GetForegroundDrawList();
    // drawList->AddLine(
    //     ImVec2(start.x, start.y), ImVec2(end.x, end.y), IM_COL32(255, 0, 0, 255), 2.0f
    // );
}

size_t World::getNumChunks() const {
    return chunks.size();
}

void World::update(float deltaTime) {
    player.update(deltaTime);

    if (player.movedChunks() || chunks.empty()) {
        glm::ivec2 worldIdx = Chunk::getWorldIndex(player.position);
        for (auto it = chunks.begin(); it != chunks.end();) {
            auto diff = glm::abs(it->first - worldIdx);
            if (diff.x > renderDistance + 2 || diff.y > renderDistance + 2) {
                it = chunks.erase(it);
            } else {
                it++;
            }
        }

        for (int z = worldIdx.y - renderDistance; z < worldIdx.y + renderDistance + 1; z++) {
            for (int x = worldIdx.x - renderDistance; x < worldIdx.x + renderDistance + 1; x++) {
                auto idx = glm::ivec2(x, z);
                if (generateChunk(idx, true).second) {
                    // LOG("Generated chunk (" << x << ", " << z << ")");
                }
            }
        }
    }
}

std::pair<World::ChunkMapType::iterator, bool>
World::generateChunk(glm::ivec2 worldIdx, bool createMesh) {
    auto result = chunks.emplace(std::make_pair(worldIdx, Chunk(worldIdx)));
    std::pair<ChunkMapType::iterator, bool> a = result;
    auto it = result.first;
    bool wasInserted = result.second;

    Chunk& chunk = it->second;

    if (!wasInserted) {
        // This line can cause random segfaults btw
        if (createMesh && !chunk.mesh) {
            chunk.mesh = std::make_unique<ChunkMesh>(chunk);
        }
        return result;
    }

    // gen terrain
    worldGen.generateTerrain(chunk);

    if (createMesh) chunk.mesh = std::make_unique<ChunkMesh>(chunk);

    return result;
}

static float sign(float x) {
    return (x > 0 ? 1 : (x < 0 ? -1 : 0));
}
static float frac0(float x) {
    return x - std::floor(x);
}
static float frac1(float x) {
    return 1 - x + std::floor(x);
}

World::RayCastResult World::castRay(const utils::Ray& ray) {
    using Direction = utils::Direction;

    RayCastResult result;

    // https://stackoverflow.com/questions/12367071/how-do-i-initialize-the-t-variables-in-a-fast-voxel-traversal-algorithm-for-ray

    float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;
    glm::ivec3 voxel;

    float x1, y1, z1; // start point
    x1 = ray.origin.x;
    y1 = ray.origin.y;
    z1 = ray.origin.z;

    glm::vec3 endPos = ray.origin + glm::normalize(ray.direction) * ray.length;
    float x2, y2, z2; // end point
    x2 = endPos.x;
    y2 = endPos.y;
    z2 = endPos.z;

    int dx = sign(x2 - x1);
    if (dx != 0)
        tDeltaX = std::abs(1.0f / (x2 - x1)); // Changed to std::abs to ensure positive step size
    else tDeltaX = 10000000.0f;
    tMaxX = (dx > 0) ? tDeltaX * frac1(x1)
                     : tDeltaX * frac0(x1); // Adjusted to use corrected frac1 and frac0
    voxel.x = (int)std::floor(x1
    ); // Changed to std::floor for correct voxel initialization with negative values

    int dy = sign(y2 - y1);
    if (dy != 0)
        tDeltaY = std::abs(1.0f / (y2 - y1)); // Changed to std::abs to ensure positive step size
    else tDeltaY = 10000000.0f;
    tMaxY = (dy > 0) ? tDeltaY * frac1(y1)
                     : tDeltaY * frac0(y1); // Adjusted to use corrected frac1 and frac0
    voxel.y = (int)std::floor(y1
    ); // Changed to std::floor for correct voxel initialization with negative values

    int dz = sign(z2 - z1);
    if (dz != 0)
        tDeltaZ = std::abs(1.0f / (z2 - z1)); // Changed to std::abs to ensure positive step size
    else tDeltaZ = 10000000.0f;
    tMaxZ = (dz > 0) ? tDeltaZ * frac1(z1)
                     : tDeltaZ * frac0(z1); // Adjusted to use corrected frac1 and frac0
    voxel.z = (int)std::floor(z1
    ); // Changed to std::floor for correct voxel initialization with negative values

    while (true) {
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                voxel.x += dx;
                tMaxX += tDeltaX;
                result.face = (dx > 0) ? Direction::WEST : Direction::EAST;

            } else {
                voxel.z += dz;
                tMaxZ += tDeltaZ;
                result.face = (dz > 0) ? Direction::NORTH : Direction::SOUTH;
            }
        } else {
            if (tMaxY < tMaxZ) {
                voxel.y += dy;
                tMaxY += tDeltaY;
                result.face = (dy > 0) ? Direction::DOWN : Direction::UP;

            } else {
                voxel.z += dz;
                tMaxZ += tDeltaZ;
                result.face = (dz > 0) ? Direction::NORTH : Direction::SOUTH;
            }
        }
        if (tMaxX > 1 && tMaxY > 1 && tMaxZ > 1) break;
        if (getBlock(voxel)) {
            // std::cout << voxel.x << ", " << voxel.y << ", " << voxel.z << "\n";
            result.hit = true;
            result.blockPosition = voxel;
            return result;
        }
    }

    result.hit = false;
    return result;
}
