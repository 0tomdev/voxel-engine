#version 330 core

out vec4 FragColor;

// in vec4 vertexColor;
in vec2 TexCoord;

uniform float time;
uniform sampler2D ourTexture;

void main()
{
  float val = (sin(time) + 1) / 2;
  vec4 newColor = vec4(val, 0.0, 1.0 - val, 1.0);
  FragColor = texture(ourTexture, TexCoord);
}