#include "HDR_Demo.h"

HDR_DEMO::HDR_DEMO()
{

}

HDR_DEMO::~HDR_DEMO()
{

}

void HDR_DEMO::InitalizeScene(GLsizei screenWidth, GLsizei screenHeight)
{
	glEnable(GL_DEPTH_TEST);

	// Light sources
	// - Positions
	lightPositions.push_back(vector3(0.0f, 0.0f, 49.5f));
	lightPositions.push_back(vector3(0.0f, 0.0f, 19.0f));
	lightPositions.push_back(vector3(0.0f, 0.0f, 30.0f));
	lightPositions.push_back(vector3(0.0f, 0.0f, 8.0f));
	// - Colors
	lightColors.push_back(glm::vec3(100.0f, 100.0f, 100.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 20.0f));
	lightColors.push_back(glm::vec3(20.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 20.0f, 0.0f));

	// Load textures
	woodTexture = ResourceManager::LoadTexture("Resources/brickwall.jpg");
	endTexture = ResourceManager::LoadTexture("Resources/theWitness.jpg");

	SetupBuffers(screenWidth, screenHeight);

	// Shaders
	shaderBloom.Compile("Shaders/HDR_Demo/bloom.vert", "Shaders/HDR_Demo/bloom.frag");
	shaderBlur.Compile("Shaders/HDR_Demo/blur.vert", "Shaders/HDR_Demo/blur.frag");
	shaderHDR.Compile("Shaders/HDR_Demo/HDR.vert", "Shaders/HDR_Demo/HDR.frag");
	shaderLight.Compile("Shaders/light_box.vert", "Shaders/light_box.frag");

	// Set up bloom shader samplers.
	shaderHDR.Use();
	// Number corresponds to the texture unit.
	glUniform1i(glGetUniformLocation(shaderHDR.Program, "scene"), 0);
	glUniform1i(glGetUniformLocation(shaderHDR.Program, "bloomBlur"), 1);
}

// NOTE(Darren): Should i have a set up buffers for all demos, really need to
// organise my demos.
// TODO(Darren): Create the final colors buffers and depth buffers for both hdr and bloom.
void HDR_DEMO::SetupBuffers(GLsizei screenWidth, GLsizei screenHeight)
{
	// Set up floating point framebuffer to render scene to
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// TODO(Darren): Consistency with comments.
	// - Create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	glGenTextures(2	, colorBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Clamp to the edge as the blur filter would otherwise sample repeated texture values.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach texture to framebuffer.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// - Create depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Ping pong framebuffer for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// Also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
}

void HDR_DEMO::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool resized)
{
	camera.ControllerMovement();

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

	if (resized)
		SetupBuffers(screenWidth, screenHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Camera collision with in the tunnel.
	// ---

	// 1. Render scene into floating point framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 500.0f);
	Matrix4 _projection;
	_projection = _projection.perspectiveProjection(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 500.0f);
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 model;
	shaderBloom.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderBloom.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderBloom.Program, "view"), 1, GL_FALSE, &view.data[0]);

	// - set lighting uniforms
	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		float lightPosData[] = {lightPositions[i].x, lightPositions[i].y, lightPositions[i].z};
		glUniform3fv(glGetUniformLocation(shaderBloom.Program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPosData[0]);
		glUniform3fv(glGetUniformLocation(shaderBloom.Program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
	}
	float data[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderBloom.Program, "viewPos"), 1, &data[0]);

	// - render tunnel
	model = Matrix4();
	model = model.translate(vector3(0.0f, 0.0f, 25.0f));
	model = model.scale(vector3(5.0f, 6.0f, 55.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderBloom.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glUniform1i(glGetUniformLocation(shaderBloom.Program, "inverse_normals"), GL_TRUE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	SceneModels::RenderCube(5.0f, 6.0f, 55.0f);

	// Show all the light sources as bright cubes
	if (renderLights)
	{
		shaderLight.Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLight.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderLight.Program, "view"), 1, GL_FALSE, &view.data[0]);
		for (GLuint i = 0; i < lightPositions.size(); i++)
		{
			model = Matrix4();
			model = model.translate(vector3(lightPositions[i]));
			model = model.scale(vector3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(shaderLight.Program, "model"), 1, GL_FALSE, &model.data[0]);
			glUniform3fv(glGetUniformLocation(shaderLight.Program, "lightColor"), 1, &lightColors[i][0]);
			SceneModels::RenderCube(0.5f, 0.5f, 0.5f);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Blur bright fragments with two-pass Gaussian Blur 
	GLboolean horizontal = true, first_iteration = true;
	GLuint amount = 2;
	shaderBlur.Use();
	for (GLuint i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(shaderBlur.Program, "horizontal"), horizontal);
		// Bind texture of other framebuffer (or scene if first iteration).
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
		SceneModels::RenderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderHDR.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	// TODO(Darren): May create a bool here.
	glUniform1i(glGetUniformLocation(shaderHDR.Program, "hdr"), true);
	glUniform1i(glGetUniformLocation(shaderHDR.Program, "bloom"), true);
	glUniform1f(glGetUniformLocation(shaderHDR.Program, "exposure"), exposure);
	SceneModels::RenderQuad();
}