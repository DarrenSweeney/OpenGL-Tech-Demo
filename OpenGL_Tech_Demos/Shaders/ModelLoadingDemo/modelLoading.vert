#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;


out VS_OUT {
	vec3 FragmentPosition;
	vec3 Normal;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentCameraPos;
	vec3 TangentFragPos;
} vs_out;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
	vs_out.FragmentPosition = vec3(modelMatrix * vec4(position, 1.0f));
	vs_out.Normal = transpose(inverse(mat3(modelMatrix))) * normal;
	vs_out.TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	vec3 B = normalize(normalMatrix * cross(T, N));
	// Take the inverse of the TBN matrix that transforms any vector from world space to tangent space
	// and use this matrix to transform not the normal, but the other relevant lighting variables
	// to tangent space. Note that we use the transpose function instead of the inverse function here.
	// A great property of orthogonal matrices (each axis is a perpendicular unit vector) is
	// that the transpose of an orthogonal matrix is equal to its inverse. 
	// This is a great property as inverses are quite expensive and a transpose isn't.
	mat3 TBN = transpose(mat3(T, B, N)); 
	vs_out.TangentLightPos = TBN * lightPosition;
	vs_out.TangentCameraPos = TBN * cameraPosition;
	vs_out.TangentFragPos = TBN * vs_out.FragmentPosition;
}