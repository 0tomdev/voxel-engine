#version 330 core

out vec4 FragColor;

in vec2 bTexCoord;
in vec3 bVertColor;
flat in int bTextureIdx;

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
    vec2 atlasPos = getAtlasPos(bTextureIdx);
    // vec2 atlasPos = vec2(2, 15);

    vec2 newTexCoord = (bTexCoord + atlasPos) / atlasSize;
    vec4 color = texture(ourTexture, newTexCoord);
    // vec4 color = vec4(1, 1, 1, 1);
    FragColor = vec4(color.xyz * bVertColor, color.w);
}