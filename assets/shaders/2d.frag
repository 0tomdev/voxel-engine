#version 330 core

in vec2 bTexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main() {
    // FragColor = vec4(1, 1, 1, 1);
    FragColor = texture(ourTexture, bTexCoord);
}