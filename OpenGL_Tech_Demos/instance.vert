#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;
/*
	When declaring a datatype as a vertex attribute that is greater than a vec4 
	thing work a bit differently. The maximum amount of data allowed as a vertex
	attribute is equal to a a vec4. Because mat4 is basically 4 vec4's, we have 
	to reserve 4 vertex attributes for this specific matrix. Because we assigned
	it a location of 3, the coulmns of the matrix will have vertex attributes 
	locations of 3, 4, 5, 6.
*/
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);
	TexCoords = texCoords;
}