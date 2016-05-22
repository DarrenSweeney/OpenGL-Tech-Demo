#include "ParallaxMappingDemo.h"

ParallaxMappingDemo::ParallaxMappingDemo()
	: enableParallax(true)
{
	
}

ParallaxMappingDemo::~ParallaxMappingDemo()
{
	
}

void ParallaxMappingDemo::Initalize(vector3 &cameraPos)
{
	cameraPos = vector3(0.0f, 0.0f, 60.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup and compile our shaders
	shaderParallax.InitShader("parallax_mapping.vert", "parallax_mapping.frag");

	// Load textures
	diffuseMap = LoadTexture("Resources/Parallax/photosculpt-graystonewall-diffuse.jpg");
	normalMap = LoadTexture("Resources/Parallax/photosculpt-graystonewall-normal.jpg");
	displacementMap = LoadTexture("Resources/Parallax/photosculpt-graystonewall-displace.jpg");

	// Set textures units
	shaderParallax.Use();
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "depthMap"), 2);

	// TODO(Darren): Create a list of color lights and move around in a scene
	// of a large area of ground.
	lightPos = vector3(0.5f, 1.0f, 0.4f);
}

void ParallaxMappingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	// Configure view/projection matrices
	shaderParallax.Use();
	Matrix4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 600.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Render normal-mapped quad
	Matrix4 model;
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, &model.data[0]);
	float lightData[] = { lightPos.x, lightPos.y, lightPos.z };
	glUniform3fv(glGetUniformLocation(shaderParallax.Program, "lightPos"), 1, &lightData[0]);
	float cameraPos[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderParallax.Program, "viewPos"), 1, &cameraPos[0]);
	glUniform1f(glGetUniformLocation(shaderParallax.Program, "height_scale"), 0.05f);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "parallax"), enableParallax);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap);
	RenderQuad();

	// render light source (simply renders a smaller plane at the light's position for debugging/visualization)
	model = Matrix4();
	model = model.translate(lightPos);
	model = model.scale(vector3(50.0f, 50.0f, 50.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, &model.data[0]);
	RenderQuad();
}

GLuint ParallaxMappingDemo::LoadTexture(GLchar *path)
{
	// Generate a texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	if (!image)
		std::cout << "ERROR:: Image was not loaded!" << std::endl;
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

void ParallaxMappingDemo::RenderQuad()
{
	if (quadVAO == 0)
	{
		// Positions
		vector3 pos1(-1.0f, 1.0f, 0.0f);
		vector3 pos2(-1.0f, -1.0f, 0.0f);
		vector3 pos3(1.0f, -1.0f, 0.0f);
		vector3 pos4(1.0f, 1.0f, 0.0f);

		// Texture coordinates
		vector2 uv1(0.0f, 1.0f);
		vector2 uv2(0.0f, 0.0f);
		vector2 uv3(1.0f, 0.0f);
		vector2 uv4(1.0f, 1.0f);

		// Normal vector
		vector3 nm(0.0f, 0.0f, 1.0f);

		// Calculating tangent/bitangent vectors of both triangels
		vector3 tangent1, bitangent1;
		vector3 tangent2, bitangent2;

		// Triangle 1
		vector3 edge1 = pos2 - pos1;
		vector3 edge2 = pos3 - pos1;
		vector2 deltaUV1 = uv2 - uv1;
		vector2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = tangent1.normalise();

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = bitangent1.normalise();

		// Triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;


		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = tangent2.normalise();


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = bitangent2.normalise();


		GLfloat quadVertices[] = {
			// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}