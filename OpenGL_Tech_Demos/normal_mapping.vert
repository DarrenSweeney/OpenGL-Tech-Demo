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
	mat3 TBN = transpose(mat3(T, B, N)); //take the inverse of the TBN matrix that transforms any vector from world space to tangent space and use this matrix to transform not the normal, but the other relevant lighting variables to tangent space. Note that we use the transpose function instead of the inverse function here. A great property of orthogonal matrices (each axis is a perpendicular unit vector) is that the transpose of an orthogonal matrix is equal to its inverse. This is a great property as inverses are quite expensive and a transpose isn't; the results are the same.
	vs_out.TangentLightPos = TBN * lightPosition;
	vs_out.TangentCameraPos = TBN * cameraPosition;
	vs_out.TangentFragPos = TBN * vs_out.FragmentPosition;
}
/*First way
We take a TBN matrix that transforms any vector from tangent to world space, give it to the fragment shader and transform the sampled normal from tangent space to world space using the TBN matrix; the normal is then in the same space as the other lighting variables.
Let's review the first case. The normal vector we sample from the normal map is expressed in tangent space whereas the other lighting vectors (light and view pos) are expressed in world space. By passing the TBN matrix to the fragment shader we can multiply the sampled tangent space normal with this TBN matrix to transform the normal vector to the same reference (world) space as the other lighting vectors. This way all the lighting calculations (specifically the dot product) make sense.*/

/*Second way
We take the inverse of the TBN matrix that transforms any vector from world space to tangent space and use this matrix to transform not the normal, but the other relevant lighting variables to tangent space; the normal is then again in the same space as the other lighting variables.
Let's also review the second case, where we take the inverse of the TBN matrix to transform all relevant world-space vectors to the space the sampled normal vectors are in: tangent space. The construction of the TBN matrix remains the same, but we first inverse the matrix before sending it to the fragment shader.
Within the fragment shader we then do not transform the normal vector, but we transform the other relevant vectors to tangent space, namely the lightDir and viewDir vectors. That way each vector is again in the same coordinate system: tangent space.
Well, transforming vectors from world to tangent space has an added advantage in that we can transform all the relevant vectors to tangent space in the vertex shader instead of in the fragment shader. This works, because lightPos and viewPos do not change each fragment run and for fs_in.FragPos we can also calculate its tangent-space position in the vertex shader and let fragment interpolation do its work. Basically, there is no need to transform any vector to tangent space in the fragment shader, while it is necessary with the first approach as sampled normal vectors are specific to each fragment shader run.

*/