#include "DeferredRenderingDemo.h"

DeferredRenderingDemo::DeferredRenderingDemo()
	: NR_Lights(300), moveLights(false), renderLights(true), initalizeScene(true)
{

}

DeferredRenderingDemo::~DeferredRenderingDemo()
{
	objectPositions.clear();
	lightPositions.clear();
	lightColors.clear();

	if (planeDiffuseTextureID)
		glDeleteTextures(1, &planeDiffuseTextureID);
	if (planeSpecularTextureID)
		glDeleteTextures(1, &planeSpecularTextureID);
	if (objectDiffuseTextureID)
		glDeleteTextures(1, &objectDiffuseTextureID);
	if (objectSpecularTextureID)
		glDeleteTextures(1, &objectSpecularTextureID);

	if (gBuffer)
		glDeleteBuffers(1, &gBuffer);
	if (gNormal)
		glDeleteBuffers(1, &gNormal);
	if (gAlbedoSpec)
		glDeleteBuffers(1, &gAlbedoSpec);
	if (rboDepth)
		glDeleteBuffers(1, &rboDepth);
}

void DeferredRenderingDemo::InitalizeScene(GLsizei screenWidth, GLsizei screenHeight)
{
	if (initalizeScene)
	{
		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);

		// Setup and compile our shaders
		shaderGeometryPass = ResourceManager::GetShader("g_buffer");
		shaderLightingPass = ResourceManager::GetShader("DeferredShading");
		shaderLightBox = ResourceManager::GetShader("LightBox");

		planeDiffuseTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-diffuse.jpg");
		planeSpecularTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-specular.jpg");
		objectDiffuseTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-diffuse.jpg");
		objectSpecularTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-specular.jpg");

		// Set samplers
		shaderLightingPass->Use();
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gPosition"), 0);
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(shaderLightingPass->Program, "gAlbedoSpec"), 2);

		shaderGeometryPass->Use();
		glUniform1i(glGetUniformLocation(shaderGeometryPass->Program, "texture_diffuse1"), 0);
		glUniform1i(glGetUniformLocation(shaderGeometryPass->Program, "texture_specular1"), 1);

		// Models
		sceneModel = ResourceManager::GetModel("Utah_Teapot");

		GLuint counter = 0;
		GLfloat zPos = -3.0f;
		for (GLuint i = 0; i < 100; i++)
		{
			objectPositions.push_back(vector3((counter * 4.0f) - 3.0f, -3.0f, zPos));

			if (counter >= 9)
			{
				zPos += 3.0f;
				counter = 0;
			}
			else
				counter++;
		}

		// - Colors
		srand(13);
		for (GLuint i = 0; i < NR_Lights; i++)
		{
			// Calculate slightly random offsets
			GLfloat xPos = ((rand() % 100) / 100.0) * 37.0 - 3.5f;
			GLfloat yPos = -1.5f;
			GLfloat zPos = ((rand() % 100) / 100.0) * 30.0 - 3.5f;
			lightPositions.push_back(vector3(xPos, yPos, zPos));
			// Also calculate random color
			GLfloat rColor = ((rand() % 100) / 200.0f); // Between 0.5 and 1.0
			GLfloat gColor = ((rand() % 100) / 200.0f); // Between 0.5 and 1.0
			GLfloat bColor = ((rand() % 100) / 200.0f); // Between 0.5 and 1.0
			lightColors.push_back(vector3(rColor, gColor, bColor));
		}

		SetupBuffers(screenWidth, screenHeight);

		initalizeScene = false;
	}
}

void DeferredRenderingDemo::SetupBuffers(GLsizei screenWidth, GLsizei screenHeight)
{
	// Set up G-Buffer
	// 3 textures:
	// 1. Positions (RGB)
	// 2. Color (RGB) + Specular (A)
	// 3. Normals (RGB) 
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// - Color and Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
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
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool resized)
{
	if (moveLights)
	{
		for (GLuint i = 0; i < lightPositions.size(); i++)
		{
			lightPositions[i].x += sin(glfwGetTime()) * 0.1f;
			lightPositions[i].z += cos(glfwGetTime()) * 0.1f;
		}
	}

	if (resized)
		SetupBuffers(screenWidth, screenHeight);

	// 1. Geometry Pass: render scene's geometry/color data into gbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 model = Matrix4();
	Matrix4 scale = Matrix4();
	Matrix4 translate = Matrix4();

	shaderGeometryPass->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "view"), 1, GL_FALSE, &view.data[0]);
	model = Matrix4();
	model = model.translate(vector3(15.0f, -3.5f, 11.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeDiffuseTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planeSpecularTextureID);
	glBindTexture(GL_TEXTURE_2D, 0);
	SceneModels::RenderPlane(25.0f, 25.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectDiffuseTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, objectSpecularTextureID);
	for (GLuint i = 0; i < objectPositions.size(); i++)
	{
		model = Matrix4();
		translate = translate.translate(objectPositions[i]);
		scale = scale.scale(vector3(0.08f, 0.08f, 0.08f));
		model = scale * translate;
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass->Program, "model"), 1, GL_FALSE, &model.data[0]);
		sceneModel->Draw(*shaderGeometryPass);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightingPass->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	// Also send light relevant uniforms
	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		// Get the light position data.
		float lightPosData[] = { lightPositions[i].x, lightPositions[i].y, lightPositions[i].z };
		// Get the light color data.
		float lightColorData[] = { lightColors[i].x, lightColors[i].y, lightColors[i].z };

		glUniform3fv(glGetUniformLocation(shaderLightingPass->Program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPosData[0]);
		glUniform3fv(glGetUniformLocation(shaderLightingPass->Program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColorData[0]);
		// Update attenuation parameters and calculate radius
		// const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		// TODO(Darren): Sort this out.
		const GLfloat linear = 0.7;
		const GLfloat quadratic = 1.8;
		glUniform1f(glGetUniformLocation(shaderLightingPass->Program, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(shaderLightingPass->Program, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
	}

	float data[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderLightingPass->Program, "viewPos"), 1, &data[0]);
	// Finally render quad
	SceneModels::RenderQuad();

	// 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	// Write to default framebuffer.
	/*
		Blit to deafult framebuffer. Note that this may or may not work as the internal formats of both the FBO and deafult
		framebuffer have to match. The internal formats are implementation defined. This works on all of my systems, but
		if it doesn't on yours you'll likely have to write to the depth buffer in another shader stage (or somehow see to
		match the default framebuffer's internal format with the FBO's internal format).
	*/
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Render lights on top of scene, by blitting.
	if (renderLights)
	{
		shaderLightBox->Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
		for (GLuint i = 0; i < lightPositions.size(); i++)
		{
			// Get the light color data.
			float lightColorData[] = { lightColors[i].x, lightColors[i].y, lightColors[i].z };

			model = Matrix4();
			translate = translate.translate(lightPositions[i]);
			scale = scale.scale(vector3(0.25f, 0.25f, 0.25f));
			model = scale * translate;
			glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, &model.data[0]);
			glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);
			SceneModels::RenderCube();
		}
	}
}