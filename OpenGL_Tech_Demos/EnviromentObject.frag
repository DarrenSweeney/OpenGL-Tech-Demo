#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseTexture;

void main()
{
	vec3 diffuse = texture(diffuseTexture, TexCoords).rgb;

	FragColor = vec4(diffuse, 1.0f);
}