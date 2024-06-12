#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in uint aNormalIdx;
layout(location = 3) in int aTexIdx;

out vec2 bTexCoord;
out vec3 bVertColor;
flat out int bTextureIdx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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

void main() {
    vec3 normal = normalArray[aNormalIdx];
    // normal *= vec3(0, 0, 1);
    bVertColor = ((normal + 1) / 2.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    bTexCoord = aTexCoord;

    bTextureIdx = aTexIdx;
}