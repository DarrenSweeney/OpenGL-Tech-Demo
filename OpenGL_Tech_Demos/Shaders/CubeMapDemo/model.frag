#version 330 core

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;
out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_reflection1;

void main()
{
	vec4 diffuse_color = texture(texture_diffuse1, TexCoords);

	// float ratio = 1.00 / 1.52;
	vec3 I = normalize(Position - cameraPos);
	vec3 R = reflect(I, normalize(Normal));		//, ratio);
	float reflection_Intensity = texture(texture_reflection1, TexCoords).r;
	vec4 reflect_color;

	if(reflection_Intensity > 0.1)
	{
		reflect_color = texture(skybox, R) * reflection_Intensity;
	}

	//vec3 Tr = refract(I, normalize(Normal), 0.7);
	//vec3 Tg = refract(I, normalize(Normal), 0.7);
	//vec3 Tb = refract(I, normalize(Normal), 0.6);
	//vec3 finalColor;
	//finalColor.r = texture(skybox, normalize(Tr)).r;
	//finalColor.g = texture(skybox, normalize(Tg)).g;
	//finalColor.b = texture(skybox, normalize(Tb)).b;

	color =	texture(skybox, R);	// diffuse_color + reflect_color;
}