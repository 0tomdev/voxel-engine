#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint normalIdx;
layout (location = 3) in int texIdx;

out vec2 TexCoord;
out vec3 vertColor;
flat out int textureIdx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec3 normalArray[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, -1.0)
);

void main()
{
	vec3 normal = normalArray[normalIdx];
    // normal *= vec3(0, 0, 1);
	vertColor = ((normal + 1) / 2.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;

    textureIdx = texIdx;
}