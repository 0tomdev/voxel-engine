#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vertColor;

uniform float time;
uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord) + vec4(vertColor, 0) * 0.3;
    // FragColor = vec4(vertColor, 1.0);
}