#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vertColor;

uniform sampler2D ourTexture;
uniform float opacity;

int textureSize = 16;
ivec2 atlasSize = ivec2(16, 16);

void main()
{
    vec4 color = texture(ourTexture, TexCoord) + vec4(vertColor, 0) * 0.0;
    FragColor = vec4(color.xyz, 1);
    // FragColor = vec4(vertColor.xyz, 1);
}