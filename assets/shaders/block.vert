#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in uint aNormalIdx;
layout(location = 3) in int aTexIdx;
layout(location = 4) in int aOcclusionValue;
layout(location = 5) in uint aIsLowered;

out vec2 bTexCoord;
out vec3 bVertColor;
flat out int bTextureIdx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform ivec3 chunkPos;

// clang-format off
const vec3 normalArray[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, -1.0)
);
// clang-format on

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() {
    const float loweredAmount = 4.0 / 16.0;

    vec3 normal = normalArray[aNormalIdx];

    vec3 vertPos = aPos;
    vec3 worldVertPos = chunkPos + vertPos;
    if (aIsLowered != uint(0)) {
        float heightOffset = (sin(time + worldVertPos.x + worldVertPos.z) + 1) / 2 / 8;
        vertPos -= vec3(0, loweredAmount + heightOffset, 0);
    }
    gl_Position = projection * view * model * vec4(vertPos, 1.0);

    bTexCoord = aTexCoord;

    // This is between 0 and 1. Lower value means darker AO.
    float aoMinMultipler = 0.5;
    bVertColor = vec3(map(aOcclusionValue, 0, 3, aoMinMultipler, 1));

    bTextureIdx = aTexIdx;
}