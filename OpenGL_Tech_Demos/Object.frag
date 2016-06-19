#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;
uniform vec3 overrideColor = vec3(1.0f, 1.0f, 1.0f);

void main()
{
	color = texture(texture1, TexCoords);
	//color *= overrideColor;
}