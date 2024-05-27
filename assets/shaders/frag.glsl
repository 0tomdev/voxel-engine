#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vertColor;

uniform sampler2D ourTexture;
uniform float opacity;

// Height and width of texture atlas, measured in number of blocks
int atlasSize = 16;

// Height and width of texture atlas image (it's a square)
int imageSize = 256;

/* Problem

 +u
  |
  |
  |
 (0,0)----- +v

*/

void main()
{
    vec2 atlasPos = vec2(3, 0);

    vec2 newTexCoord = TexCoord / atlasSize;
    // newTexCoord += atlasPos / 16;
    // newTexCoord.y = 1 - newTexCoord.y;
    vec4 color = texture(ourTexture, newTexCoord) + vec4(vertColor, 0) * 0.0;
    FragColor = vec4(color.xyz, 1);
    // FragColor = vec4(vertColor.xyz, 1);
}