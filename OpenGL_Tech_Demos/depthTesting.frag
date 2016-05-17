#version 330 core

in vec2 TexCoords;

out vec4 color;

float near = 0.1;
float far = 100.0;

uniform sampler2D texture1;
uniform vec3 overrideColor = vec3(1.0f, 1.0f, 1.0f);

float LinearizedDepth(float depth)
{
	float z = depth * 2.0 - 1.0;	// Back to Normalized Device coodinates.
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	//float depth = LinearizedDepth(gl_FragCoord.z) / far;
	//color = vec4(vec3(depth), 1.0);
	color = texture(texture1, TexCoords);
	color *= overrideColor;
}
