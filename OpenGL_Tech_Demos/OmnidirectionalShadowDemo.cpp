#include "OmnidirectionalShadowDemo.h"

OmnidirectionalShadowDemo::OmnidirectionalShadowDemo()
	: ShadowWidth(1024), ShadowHeight(1024), moveLight(false), renderLight(false), initalizeScene(true)
{

}

OmnidirectionalShadowDemo::~OmnidirectionalShadowDemo()
{
	if (woodTextureID)
		glDeleteTextures(1, &woodTextureID);
	if (wallTextureID)
		glDeleteTextures(1, &wallTextureID);
	if (depthMapFBO)
		glDeleteBuffers(1, &depthMapFBO);
	if (depthCubemap)
		glDeleteTextures(1, &depthCubemap);
}

void OmnidirectionalShadowDemo::Initalize()
{
	if (initalizeScene)
	{
		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Set texture samplers for point shadows
		shaderPointShadows = ResourceManager::GetShader("PointShadows");
		shaderPointShadows->Use();
		glUniform1i(glGetUniformLocation(shaderPointShadows->Program, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shaderPointShadows->Program, "depthMap"), 1);
		shaderPointShadowsDepth = ResourceManager::GetShader("PointShadowsDepth");
		shaderLightBox = ResourceManager::GetShader("LightBox");

		// Light source
		lightPosition = vector3();

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
	if (moveLight)
	{
		lightPosition.x = sin(glfwGetTime()) * 4.0f;
		lightPosition.z = cos(glfwGetTime()) * 4.0f;
		lightPosition.y = cos(glfwGetTime()) * 1.0f;
	}

	// 1. Create depth cubemap transformation matrices
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 25.0f;
	GLfloat aspect = (GLfloat)ShadowWidth / (GLfloat)ShadowHeight;
	Matrix4 shadowProjection = Matrix4();
	shadowProjection.perspectiveProjection(90.0f, aspect, nearPlane, farPlane);
	std::vector<Matrix4> shadowTrans;
	Matrix4 LookAt = Matrix4();
	//																	   Target vectors				 Up vectors
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(1.0f, 0.0f, 0.0f), vector3(0.0f, -1.0f, 0.0f)) * shadowProjection);
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(-1.0f, 0.0f, 0.0f), vector3(0.0f, -1.0f, 0.0f)) * shadowProjection);
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(0.0f, 1.0f, 0.0f), vector3(0.0f, 0.0f, 1.0f)) * shadowProjection);
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(0.0f, -1.0f, 0.0f), vector3(0.0f, 0.0f, -1.0f)) * shadowProjection);
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(0.0f, 0.0f, 1.0f), vector3(0.0f, -1.0f, 0.0f)) * shadowProjection);
	shadowTrans.push_back(LookAt.lookAt(lightPosition, lightPosition + vector3(0.0f, 0.0f, -1.0f), vector3(0.0f, -1.0f, 0.0f)) *  shadowProjection);

	// 2. Render scene to depth cubemap
	glViewport(0, 0, ShadowWidth, ShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderPointShadowsDepth->Use();
	for (GLuint i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(shaderPointShadowsDepth->Program, ("shadowMatrices[" + std::to_string(i) + "]").c_str()),
			1, GL_FALSE, &shadowTrans[i].data[0]);
	glUniform1f(glGetUniformLocation(shaderPointShadowsDepth->Program, "far_plane"), farPlane);
	float lightPosData[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	glUniform3fv(glGetUniformLocation(shaderPointShadowsDepth->Program, "lightPos"), 1, &lightPosData[0]);
	RenderScene(*shaderPointShadowsDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. Render scene as normal 
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderPointShadows->Use();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
	Matrix4 view;
	view = camera.GetViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shaderPointShadows->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderPointShadows->Program, "view"), 1, GL_FALSE, &view.data[0]);
	// Set light uniforms
	glUniform3fv(glGetUniformLocation(shaderPointShadows->Program, "lightPos"), 1, &lightPosData[0]);
	float cameraPosData[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderPointShadows->Program, "viewPos"), 1, &cameraPosData[0]);
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
		Matrix4 model = Matrix4();
		model = model.scale(vector3(0.5f, 0.5f, 0.5f));
		model = model.translate(lightPosition);
		shaderLightBox->Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, &model.data[0]);
		glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);
		SceneModels::RenderCube();
	}
}

void OmnidirectionalShadowDemo::RenderScene(Shader &shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTextureID);

	Matrix4 model = Matrix4();
	Matrix4 scale = Matrix4();
	Matrix4 rotate = Matrix4();
	Matrix4 translate = Matrix4();

	scale = scale.scale(vector3(40.0f, 0.2f, 40.0f));
	translate = translate.translate(vector3(0.0f, -30.5f, 0.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();

	// Render Stand
	model = Matrix4();
	scale = scale.scale(vector3(5.0f, 1.0f, 5.0f));
	translate = translate.translate(vector3(0.0f, -5.0f, 0.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();

	model = Matrix4();
	scale = scale.scale(vector3(6.0f, 6.0f, 6.0f));
	translate = translate.translate(vector3(0.0f, -1.0f, 0.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelPlatform->Draw(shader);

	model = Matrix4();
	scale = scale.scale(vector3(6.0f, 6.0f, 6.0f));
	rotate = rotate.rotateX(MathHelper::DegressToRadians(180.0f));
	translate = translate.translate(vector3(0.0f, -0.4f, 0.0f));
	model = translate * rotate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelPlatform->Draw(shader);

	// Draw Columns
	model = Matrix4();
	scale = scale.scale(vector3(0.5f, 0.5f, 0.5f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	translate = translate.translate(vector3(-9.0f, -11.5f, -9.0f));
	model = translate * rotate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelColumn->Draw(shader);

	model = Matrix4();
	scale = scale.scale(vector3(0.5f, 0.5f, 0.5f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	translate = translate.translate(vector3(9.0f, -11.5f, -9.0f));
	model = translate * rotate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelColumn->Draw(shader);

	model = Matrix4();
	scale = scale.scale(vector3(0.5f, 0.5f, 0.5f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	translate = translate.translate(vector3(-9.0f, -11.5f, 9.0f));
	model = translate * rotate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelColumn->Draw(shader);

	model = Matrix4();
	scale = scale.scale(vector3(0.5f, 0.5f, 0.5f));
	translate = translate.translate(vector3(9.0f, -11.5f, 9.0f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	model = translate * rotate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelColumn->Draw(shader);

	model = Matrix4();
	scale = scale.scale(vector3(1.5f, 1.5f, 1.5f));
	translate = translate.translate(vector3(0.0f, -3.0f, 0.0f));
	model = translate * scale;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE,  &model.data[0]);
	modelBunny->Draw(shader);
}