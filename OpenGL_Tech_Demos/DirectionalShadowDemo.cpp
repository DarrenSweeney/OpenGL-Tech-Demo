#include "DirectionalShadowDemo.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

DirectionalShadowDemo::DirectionalShadowDemo()
	: moveLight(false), ShadowWidth(1024), ShadowHeight(1024), initalizeScene(true)
{

}

DirectionalShadowDemo::~DirectionalShadowDemo()
{
	// TODO(Darren): Delete the buffers.
}

void DirectionalShadowDemo::InitalizeScene()
{
	if (initalizeScene)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		shaderShadowMap = ResourceManager::GetShader("ShadowMapping");
		shaderDepth = ResourceManager::GetShader("ShadowMappingDepth");
		shaderLightBox = ResourceManager::GetShader("LightBox");

		shaderShadowMap->Use();
		glUniform1i(glGetUniformLocation(shaderShadowMap->Program, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shaderShadowMap->Program, "shadowMap"), 1);

		// Light source
		lightPosition = vector3(-2.0f, 4.0f, -1.0f);

		// Load textures
		floorTextureID = ResourceManager::LoadTexture("Resources/brickwall.jpg", false);
		cubeTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/Creeper/photosculpt-creeper-diffuse.jpg", false);
		teapotTextureID = ResourceManager::LoadTexture("Resources/Parallax/photosculpt-graystonewall-diffuse.jpg", false);

		// Load the scene models.
		modelTree = ResourceManager::GetModel("Tree");
		modelTeapot = ResourceManager::GetModel("Utah_Teapot");
		modelRock = ResourceManager::GetModel("Rock");

		// Configure depth map FBO
		glGenFramebuffers(1, &depthMapFBO);
		// - Create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		initalizeScene = false;
	}
}

void DirectionalShadowDemo::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	// NOTE(Darren): From the CPU dignostic test _glfwPlatformJoystickPresent is using 71.09% 
	// of 74.18%, why is the joystick using so much resources?
	// TODO(Darren): Fix this issue and see if the CPU usage dramaticly does down.
	//camera.ControllerMovement();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Change light position over time
	if (moveLight)
	{
		lightPosition.x = sin(glfwGetTime()) * 3.0f;
		lightPosition.z = cos(glfwGetTime()) * 3.0f;
		lightPosition.y = 5.0 + cos(glfwGetTime()) * 1.0f;
	}

	// 1. Render depth of scene to texture (from ligth's perspective)
	// - Get light projection/view matrix.
	Matrix4 lightProjection, lightView;
	Matrix4 lightSpaceMatrix;
	GLfloat near_plane = 1.0f, far_plane = 10.0f;
	lightProjection = lightProjection.orthographicProjection(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = lightView.lookAt(lightPosition, vector3(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 1.0f));
	lightSpaceMatrix = lightView * lightProjection;
	// - now render scene from light's point of view
	shaderDepth->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderDepth->Program, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.data);

	glViewport(0, 0, ShadowWidth, ShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderScene(*shaderDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render scene as normal 
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderShadowMap->Use();
	glm::mat4 projection = glm::perspective(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	Matrix4 _projection = _projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	//glm::mat4 view = camera.getViewMatrix();
	Matrix4 view = camera.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shaderShadowMap->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderShadowMap->Program, "view"), 1, GL_FALSE, view.data);
	// Set light uniforms
	float lightPos[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	glUniform3fv(glGetUniformLocation(shaderShadowMap->Program, "lightPos"), 1, &lightPos[0]);
	float cameraPos[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderShadowMap->Program, "viewPos"), 1, &cameraPos[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderShadowMap->Program, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.data);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderScene(*shaderShadowMap);

	// Render the light.
	if (renderLight)
	{
		vector3 lightColor = vector3(1.0f, 1.0f, 1.0f);
		float lightColorData[] = { lightColor.x, lightColor.y, lightColor.z };
		Matrix4 model = Matrix4();
		model = model.scale(vector3(0.5f, 0.5f, 0.5f));
		model = model.translate(lightPosition);
		shaderLightBox->Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, &model.data[0]);
		glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);
		SceneModels::RenderCube();
	}
}

void DirectionalShadowDemo::RenderScene(Shader &shader)
{
	// Floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTextureID);
	Matrix4 model;
	model = model.translate(vector3(0.0f, -0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderPlane(25.0f, 25.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTextureID);
	model = Matrix4();
	model = model.translate(vector3(0.0f, -1.0f, 2.0f));
	model = model.scale(vector3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelTree->Draw(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, teapotTextureID);
	model = Matrix4();
	model = model.scale(vector3(0.05f, 0.05f, 0.05f));
	model = model.translate(vector3(-1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelTeapot->Draw(shader); 
	glBindTexture(GL_TEXTURE_2D, 0);

	model = Matrix4();
	model = model.translate(vector3(2.5f, -0.3f, 2.2f));
	model = model.scale(vector3(0.9f, 0.9f, 0.9f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelRock->Draw(shader);
}

// TODO(Darren): This should be in resource manager.
GLuint DirectionalShadowDemo::GenerateMultiSampleTexture(GLuint samples)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, 1100, 600, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	return texture;
}