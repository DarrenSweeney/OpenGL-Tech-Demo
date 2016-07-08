#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;

out vec2 TexCoords;

void main()
{
    gl_Position = model * vec4(position, 1.0);
	TexCoords = texCoords;
}