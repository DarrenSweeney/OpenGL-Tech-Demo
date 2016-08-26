#include "SSAO_Demo.h"

SSAO_Demo::SSAO_Demo()
	: initalizeScene(true), kernelSize(64), radius(1.0f), noiseScale(4.0f)
{

}

SSAO_Demo::~SSAO_Demo()
{
	if (noiseTexture)
		glDeleteTextures(1, &noiseTexture);

	// Delete all the allocated buffers.
	if(gBuffer)
		glDeleteBuffers(1, &gBuffer);
	if (gPositionDepth)
		glDeleteTextures(1, &gPositionDepth);
	if(gNormal)
		glDeleteTextures(1, &gNormal);
	if (gAlbedo)
		glDeleteTextures(1, &gAlbedo);
	if (rboDepth)
		glDeleteBuffers(1, &rboDepth);
	if (ssaoFBO)
		glDeleteBuffers(1, &ssaoFBO);
	if (ssaoBlurFBO)
		glDeleteBuffers(1, &ssaoBlurFBO);
	if (ssaoColorBuffer)
		glDeleteTextures(1, &ssaoColorBuffer);
	if (ssaoColorBufferBlur)
		glDeleteTextures(1, &ssaoColorBufferBlur);
}

void SSAO_Demo::InitalizeScene(GLsizei screenWidth, GLsizei screenHeight)
{
	if (initalizeScene)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Setup and compile our shaders
		shaderGeometryPass = ResourceManager::GetShader("ssao_geometry");
		shaderLightingPass = ResourceManager::GetShader("ssao_lighting");
		shaderSSAO = ResourceManager::GetShader("ssao");
		shaderSSAOBlur = ResourceManager::GetShader("ssao_blur");

		// Set samplers
		shaderLightingPass->Use();
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gPositionDepth"), 0);
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gAlbedo"), 2);
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "ssao"), 3);
		shaderSSAO->Use();
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "diffuse"), 5);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "gPositionDepth"), 0);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "texNoise"), 2);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "screenWidth"), screenWidth);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "screenHeight"), screenHeight);

		modelSphere = ResourceManager::GetModel("Sphere");
		modelMonkey = ResourceManager::GetModel("Monkey");
		modelStatue = ResourceManager::GetModel("Statue");

		// Lights
		lightPos = vector3(2.0f, 4.0f, 0.0f);
		lightColor = vector3(1.0f, 1.0f, 1.0f);

		SetupBuffers(screenWidth, screenHeight);

		// Sample kernel
		randomFloats = std::uniform_real_distribution<GLfloat>(0.0, 1.0); // generates random floats between 0.0 and 1.0

		for (GLuint i = 0; i < 64; ++i)
		{
			vector3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = sample.normalise();
			sample *= randomFloats(generator);
			GLfloat scale = GLfloat(i) / 64.0;

			// Scale samples s.t. they're more aligned to center of kernel
			scale = Lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		// Noise texture
		for (GLuint i = 0; i < 16; i++)
		{
			// Rotate around z-axis (in tangent space)
			vector3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
			ssaoNoise.push_back(noise);
		}

		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		initalizeScene = false;
	}
}

void SSAO_Demo::SetupBuffers(GLsizei screenWidth, GLsizei screenHeight)
{
	// Set up G-Buffer
	// 3 textures:
	// 1. Positions + depth (RGBA)
	// 2. Color (RGB) 
	// 3. Normals (RGB) 
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// - Position + linear depth color buffer
	glGenTextures(1, &gPositionDepth);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// - Albedo color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// - Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "GBuffer Framebuffer not complete!" << std::endl;

	// Also create framebuffer to hold SSAO processing stage 
	glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	// - SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	// - and blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLfloat SSAO_Demo::Lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}

void SSAO_Demo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool windowResized)
{
	if (windowResized)
	{
		SetupBuffers(screenWidth, screenHeight);

		shaderSSAO->Use();
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "screenWidth"), screenWidth);
		glUniform1i(glGetUniformLocation(shaderSSAO->Program, "screenHeight"), screenHeight);
	}

	// 1. Geometry Pass: render scene's geometry/color data into gbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Matrix4 projection = Matrix4();
	projection = projection.perspectiveProjection(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 50.0f);
	Matrix4 view = camera.GetViewMatrix();
	shaderGeometryPass->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "view"), 1, GL_FALSE, &view.data[0]);
	// Floor cube
	Matrix4 model = Matrix4();
	Matrix4 scale = Matrix4();
	Matrix4 translate = Matrix4();
	Matrix4 rotate = Matrix4();
	translate = translate.translate(vector3(0.0, -1.0f, 0.0f));
	scale = scale.scale(vector3(20.0f, 1.0f, 20.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	model = Matrix4();
	translate = translate.translate(vector3(-2.0f, 0.0f, 0.0f));
	scale = scale.scale(vector3(3.0f, 1.0f, 3.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	// Render model on the floor
	model = Matrix4();
	translate = translate.translate(vector3(0.0f, 0.5f, 10.0f));
	scale = scale.scale(vector3(0.5f, 0.5f, 0.5f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelSphere->Draw(*shaderGeometryPass);
	model = Matrix4();
	translate = translate.translate(vector3(-3.7f, 0.5f, -1.0f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	model = rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelMonkey->Draw(*shaderGeometryPass);
	model = Matrix4();
	translate = translate.translate(vector3(80.0f, -15.0f, -125.0f));
	scale = scale.scale(vector3(0.04f, 0.04f, 0.04f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelStatue->Draw(*shaderGeometryPass);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Create SSAO texture
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAO->Use();
	glUniform1i(glGetUniformLocation(shaderSSAO->Program, "kernelSize"), kernelSize);
	glUniform1f(glGetUniformLocation(shaderSSAO->Program, "radius"), radius);
	glUniform1f(glGetUniformLocation(shaderSSAO->Program, "noiseScaleCount"), noiseScale);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	// Send kernel + rotation 
	for (GLuint i = 0; i < 64; ++i)
	{
		GLfloat ssaoKernelData[] = {ssaoKernel[i].x, ssaoKernel[i].y, ssaoKernel[i].z};	
		glUniform3fv(glGetUniformLocation(shaderSSAO->Program, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernelData[0]);
	}
	glUniformMatrix4fv(glGetUniformLocation(shaderSSAO->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	SceneModels::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Blur SSAO texture to remove noise
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAOBlur->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	SceneModels::RenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. Lighting Pass: traditional deferred Blinn-Phong lighting now with added screen-space ambient occlusion
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightingPass->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3); 
	// Add extra SSAO texture to lighting pass
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	// Also send light relevant uniforms 
	vector4 lightPosView_ = camera.GetViewMatrix() * vector4(lightPos.x, lightPos.y, lightPos.z, 1.0);
	float lightPosViewData[] = { lightPosView_.x, lightPosView_.y , lightPosView_.z };
	glUniform3fv(glGetUniformLocation(shaderLightingPass->Program, "light.Position"), 1, &lightPosViewData[0]);
	GLfloat lightColorData[] = { lightColor.x, lightColor.y, lightColor.z };
	glUniform3fv(glGetUniformLocation(shaderLightingPass->Program, "light.Color"), 1, &lightColorData[0]);
	// Update attenuation parameters
	const GLfloat linear = 0.09;
	const GLfloat quadratic = 0.032;
	glUniform1f(glGetUniformLocation(shaderLightingPass->Program, "light.Linear"), linear);
	glUniform1f(glGetUniformLocation(shaderLightingPass->Program, "light.Quadratic"), quadratic);
	SceneModels::RenderQuad();
}