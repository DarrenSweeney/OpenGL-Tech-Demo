#include "ModelLoadingDemo.h"

ModelLoadingDemo::ModelLoadingDemo()
{

}

ModelLoadingDemo::~ModelLoadingDemo()
{
	// TODO(Darren):
	// Dealocation? >>> glBindVertexArray(0);
	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//					glBindBuffer(GL_ARRAY_BUFFER, 0);
	//					glDeleteTextures(1, &diffuseMap);
	//					glDeleteTextures(1, &normalMap);
}

// TODO(Darren): May add tangent space draw opition in my demo menu.
void ModelLoadingDemo::Initalize()
{
#pragma region Skybox Vertices
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
#pragma endregion 

	glEnable(GL_DEPTH_TEST);

	shaderModel.InitShader("modelLoading.vert", "modelLoading.frag");
	shaderSkyBox.InitShader("Shaders/CubeMapDemo/skybox.vert", "Shaders/CubeMapDemo/skybox.frag");
	shaderNormal.InitShader("normal.vert", "normal.frag", "normal.gs");
	sceneModel.LoadModel("cyborg/cyborg.obj", true);
	lightPosition = vector3(0.0f, 0.0f, -5.0f);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindVertexArray(0);

	faces.push_back("Resources/skybox/posx.jpg");
	faces.push_back("Resources/skybox/negx.jpg");
	faces.push_back("Resources/skybox/posy.jpg");
	faces.push_back("Resources/skybox/negy.jpg");
	faces.push_back("Resources/skybox/posz.jpg");
	faces.push_back("Resources/skybox/negz.jpg");

	cubeMapTexture = LoadCubeMap(faces);

	shaderModel.Use();
	shaderModelMatrix = glGetUniformLocation(shaderModel.Program, "modelMatrix");
	shaderProjectionMatrix = glGetUniformLocation(shaderModel.Program, "projectionMatrix");
	shaderViewMatrix = glGetUniformLocation(shaderModel.Program, "viewMatrix");
	shaderCameraPos = glGetUniformLocation(shaderModel.Program, "cameraPosition");
	shaderNormalMapping = glGetUniformLocation(shaderModel.Program, "normalMapping");
	shaderInTangentSpace = glGetUniformLocation(shaderModel.Program, "inTangentSpace");

	shaderNormalModel = glGetUniformLocation(shaderNormal.Program, "model");
	shaderNormalProjection = glGetUniformLocation(shaderNormal.Program, "projection");
	shaderNormalView = glGetUniformLocation(shaderNormal.Program, "view");
}

// TODO(Darren): Need to draw the lights, maybe a simple texture. 
// In resource manager allow call for drawing a light texture.
void ModelLoadingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	shaderModel.Use();
	Matrix4 viewMatrix = camera.GetViewMatrix();
	// TODO(Darren): Takes glm out.
	glm::mat4 projMatrix = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(shaderViewMatrix, 1, GL_FALSE, &viewMatrix.data[0]);
	glUniformMatrix4fv(shaderProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniformMatrix4fv(shaderModelMatrix, 1, GL_FALSE, &modelMatrix.data[0]);

	float lightPosData[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	//glUniform3fv(glGetUniformLocation(shaderModel.Program, "lightPosition"), 1, &lightPosData[0]);
	Matrix4 lightMatrix = lightMatrix.translate(lightPosition);
	lightMatrix = lightMatrix.scale(vector3(0.1f, 0.1f, 0.1f));

	float cameraPosData[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(shaderCameraPos, 1, &cameraPosData[0]);
	glUniform1i(shaderNormalMapping, normalMapping);
	glUniform1i(shaderInTangentSpace, inTangentSpace);

	// TODO(Darren): Need to tidy this up.
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	sceneModel.Draw(shaderModel);

	if (showNormals)
	{
		shaderNormal.Use();
		glUniformMatrix4fv(shaderNormalView, 1, GL_FALSE, &viewMatrix.data[0]);
		glUniformMatrix4fv(shaderNormalProjection, 1, GL_FALSE, glm::value_ptr(projMatrix));
		glUniformMatrix4fv(shaderNormalModel, 1, GL_FALSE, &modelMatrix.data[0]);
		sceneModel.Draw(shaderNormal);
	}

	if (showCubemap)
	{
		glDepthFunc(GL_LEQUAL);
		shaderSkyBox.Use();
		Matrix4 view = camera.GetViewMatrix();
		view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "view"), 1, GL_FALSE, view.data);
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

		// skybox 
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}
}

GLuint ModelLoadingDemo::LoadCubeMap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}