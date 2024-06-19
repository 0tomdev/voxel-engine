#version 330 core

layout(location = 0) in ivec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 bTexCoord;

uniform uvec2 screenSize;

void main() {
    vec2 coords = vec2(aPos) / vec2(screenSize) * 2.0 - vec2(1, 1);
    coords.y *= -1;

    gl_Position = vec4(coords, 0, 1);
    bTexCoord = aTexCoord;
}