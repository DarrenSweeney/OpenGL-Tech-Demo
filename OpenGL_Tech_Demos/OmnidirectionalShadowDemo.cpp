#include "OmnidirectionalShadowDemo.h"

OmnidirectionalShadowDemo::OmnidirectionalShadowDemo()
	: ShadowWidth(1024), ShadowHeight(1024), moveLight(false), initalizeScene(true)
{

}

OmnidirectionalShadowDemo::~OmnidirectionalShadowDemo()
{

}

void OmnidirectionalShadowDemo::Initalize()
{
	if (initalizeScene)
	{
		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Set texture samplers for point shadows
		// TODO(Darren): Really need to implement error check if shader is not found
		shaderPointShadows = ResourceManager::GetShader("PointShadows");
		shaderPointShadows->Use();
		glUniform1i(glGetUniformLocation(shaderPointShadows->Program, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shaderPointShadows->Program, "depthMap"), 1);
		shaderPointShadowsDepth = ResourceManager::GetShader("PointShadowsDepth");
		shaderLightBox = ResourceManager::GetShader("LightBox");
		shaderCubeMap = ResourceManager::GetShader("Skybox");
		shaderCubeMap->Use();
		glUniform1i(glGetUniformLocation(shaderCubeMap->Program, "skybox"), 0);

		// Light source
		lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

		// Load textures
		woodTextureID = ResourceManager::LoadTexture("Resources/brickwall.jpg");
		wallTextureID = ResourceManager::LoadTexture("Resources/brickwall.jpg");

		// Get the scene models
		modelPlatform = ResourceManager::GetModel("Platform");
		modelColumn = ResourceManager::GetModel("Column");
		modelBunny = ResourceManager::GetModel("Bunny");

		// Configure depth map FBO
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		// Create depth cubemap texture
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		initalizeScene = false;
	}
}

void OmnidirectionalShadowDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	if (moveLight)
	{
		lightPos.x = sin(glfwGetTime()) * 4.0f;
		lightPos.z = cos(glfwGetTime()) * 4.0f;
		lightPos.y = cos(glfwGetTime()) * 1.0f;
	}

	// 1. Create depth cubemap transformation matrices
	GLfloat aspect = (GLfloat)ShadowWidth / (GLfloat)ShadowHeight;
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	// 2. Render scene to depth cubemap
	glViewport(0, 0, ShadowWidth, ShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	shaderPointShadowsDepth->Use();
	for (GLuint i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(shaderPointShadowsDepth->Program, ("shadowMatrices[" + std::to_string(i) + "]").c_str()),
			1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	glUniform1f(glGetUniformLocation(shaderPointShadowsDepth->Program, "far_plane"), farPlane);
	glUniform3fv(glGetUniformLocation(shaderPointShadowsDepth->Program, "lightPos"), 1, &lightPos[0]);
	RenderScene(*shaderPointShadowsDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Render scene as normal 
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderPointShadows->Use();
	glm::mat4 projection = glm::perspective(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	Matrix4 view;
	view = camera.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shaderPointShadows->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderPointShadows->Program, "view"), 1, GL_FALSE, &view.data[0]);
	// Set light uniforms
	glUniform3fv(glGetUniformLocation(shaderPointShadows->Program, "lightPos"), 1, &lightPos[0]);
	glm::vec3 cameraPos(camera.position.x, camera.position.y, camera.position.z);
	glUniform3fv(glGetUniformLocation(shaderPointShadows->Program, "viewPos"), 1, &cameraPos[0]);
	glUniform1i(glGetUniformLocation(shaderPointShadows->Program, "shadows"), true);
	glUniform1f(glGetUniformLocation(shaderPointShadows->Program, "far_plane"), farPlane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	RenderScene(*shaderPointShadows);

	// Render the light.
	if (renderLight)
	{
		vector3 lightColor = vector3(1.0f, 1.0f, 1.0f);
		float lightColorData[] = { lightColor.x, lightColor.y, lightColor.z };
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, lightPos);
		shaderLightBox->Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);
		SceneModels::RenderCube();
	}

	// skybox 
	shaderCubeMap->Use();
	view = camera.GetViewMatrix();
	view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
	glUniformMatrix4fv(glGetUniformLocation(shaderCubeMap->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderCubeMap->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	SceneModels::RenderSkybox();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OmnidirectionalShadowDemo::RenderScene(Shader &shader)
{
	// Render a flat cube as floor.
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, wallTextureID);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -6.0f, 0.0));
	model = glm::scale(model, glm::vec3(40.0f, 0.2f, 40.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	SceneModels::RenderCube();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTextureID);
	// Render Stand.
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0));
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	SceneModels::RenderCube();

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(6.0f));
	//model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelPlatform->Draw(shader);

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(6.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelPlatform->Draw(shader);

	// Draw Columns
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, glm::vec3(-9.0f, -11.5f, -9.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelColumn->Draw(shader);

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, glm::vec3(9.0f, -11.5f, -9.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelColumn->Draw(shader);

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, glm::vec3(-9.0f, -11.5f, 9.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelColumn->Draw(shader);

	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::translate(model, glm::vec3(9.0f, -11.5f, 9.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelColumn->Draw(shader);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	modelBunny->Draw(shader);
}