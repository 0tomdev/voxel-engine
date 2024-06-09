#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vertColor;
flat in int textureIdx;

uniform sampler2D ourTexture;
uniform float opacity;

// Height and width of texture atlas, measured in number of blocks
int atlasSize = 16;

// Height and width of texture atlas image (it's a square)
int imageSize = 256;

ivec2 getAtlasPos(int idx) {
    ivec2 result;
    result.y = idx / atlasSize;
    result.y = atlasSize - 1 - result.y;
    result.x = idx % atlasSize;
    return result;
}

/*
 +u
  |
  |
  |
 (0,0)----- +v
*/

void main() {
    vec2 atlasPos = getAtlasPos(textureIdx);
    // vec2 atlasPos = vec2(2, 15);

    vec2 newTexCoord = (TexCoord + atlasPos) / atlasSize;
    vec4 color = texture(ourTexture, newTexCoord);
    FragColor = color;
}