#include "CubeMapDemo.h"

CubeMapDemo::CubeMapDemo()
	: initalizeScene(true)
{
	modelOrigin = vector3(0.0f, 0.0f, 0.0f);
	bluePos = vector3(4.0f, 5.0f, 1.0f);
	yellowPos = vector3(-5.0f, -1.0f, 1.0f);
	greenPos = vector3(4.5f, -5.0f, 1.5f);
	pinkPos = vector3(-3.5f, 7.0f, 5.0f);
	redPos = vector3(15.0f, -3.5f, 0.0f);
}

CubeMapDemo::~CubeMapDemo()
{
	if (redTex)
		glDeleteTextures(1, &redTex);
	if (blueTex)
		glDeleteTextures(1, &blueTex);
	if (yellowTex)
		glDeleteTextures(1, &yellowTex);
	if (greenTex)
		glDeleteTextures(1, &greenTex);
	if (pinkTex)
		glDeleteTextures(1, &pinkTex);
	if (cubeMapTextureID)
		glDeleteTextures(1, &cubeMapTextureID);
	if (woodTextureID)
		glDeleteTextures(1, &woodTextureID);
}

void CubeMapDemo::InitalizeScene(GLsizei screenWidth, GLsizei screenHeight)
{
	if (initalizeScene)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		shaderModel = ResourceManager::GetShader("Model");
		shaderSkyBox = ResourceManager::GetShader("Skybox");
		shaderEnviromentObject = ResourceManager::GetShader("EnviromentObject");

		// Set up texture sampler unit.
		shaderEnviromentObject->Use();
		glUniform1i(glGetUniformLocation(shaderEnviromentObject->Program, "diffuseTexture"), 1);

		modelUtahTeaPot = ResourceManager::GetModel("Utah_Teapot");
		modelSphere = ResourceManager::GetModel("Sphere");
		modelBunny = ResourceManager::GetModel("Monkey");

		/*
			Order is based on the Layered Rendering specfic order.
			https://www.opengl.org/wiki/Cubemap_Texture
		*/
		faces.push_back("Resources/skybox/posx.jpg");
		faces.push_back("Resources/skybox/negx.jpg");
		faces.push_back("Resources/skybox/posy.jpg");
		faces.push_back("Resources/skybox/negy.jpg");
		faces.push_back("Resources/skybox/posz.jpg");
		faces.push_back("Resources/skybox/negz.jpg");

		cubeMapTextureID = ResourceManager::LoadCubeMap(faces);
		woodTextureID = ResourceManager::LoadTexture("Resources/brickwall.jpg");

		// Debug texture IDs
		redTex = ResourceManager::LoadTexture("Resources/redTex.png");
		blueTex = ResourceManager::LoadTexture("Resources/blueTex.png");
		yellowTex = ResourceManager::LoadTexture("Resources/yellowTex.png");
		greenTex = ResourceManager::LoadTexture("Resources/greenTex.png");
		pinkTex = ResourceManager::LoadTexture("Resources/pinkTex.png");

		/*
			Create dynamic cubemap
		*/	
		// Create empty cubemap
		glGenTextures(1, &dynamicCubeTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Allocate space for each side of the cube map
		for (GLuint i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubeMapSize,
				cubeMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		// Create framebuffer
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapSize, cubeMapSize);
		// Attach one of the faces of the cubemap texture to current framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);
		// Attach depth buffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
		// Attach only the +X cubemap texture (for completeness)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);

		// Check if current configuration of framebuffer is correct
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		// Set default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		initalizeScene = false;
	}
}

void CubeMapDemo::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, GLfloat deltaTime)
{
	// Set up texture sampler unit.
	//shaderEnviromentObject->Use();
	glUniform1i(glGetUniformLocation(shaderEnviromentObject->Program, "diffuseTexture"), 1);

	if (animateScene)
	{
		redPos.y += cos(glfwGetTime()) * 3.0f * deltaTime;

		bluePos.x += sin(glfwGetTime()) * 3.0f * deltaTime;
		bluePos.z += cos(glfwGetTime()) * 3.0f * deltaTime;

		yellowPos.y += sin(glfwGetTime()) * 3.0f * deltaTime;
		yellowPos.z += cos(glfwGetTime()) * 3.0f * deltaTime;

		greenPos.x += sin(glfwGetTime()) * 3.0f * deltaTime;
		greenPos.z += cos(glfwGetTime()) * 3.0f * deltaTime;

		pinkPos.x += sin(glfwGetTime()) * 3.0f * deltaTime;
		pinkPos.y += cos(glfwGetTime()) * 3.0f * deltaTime;
	}

	// Render scene to cubemap
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 100.0f;
	glViewport(0, 0, cubeMapSize, cubeMapSize); // Set size of the viewport as size of cube map
	Matrix4 view;
	Matrix4 projection;
	Matrix4 model;
	projection = projection.perspectiveProjection(90.0f, 1.0f, nearPlane, farPlane);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); // Bind FBO to render to the texture

	for (GLuint face = 0; face < 6; ++face)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, dynamicCubeTex, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		vector3 targetVectors[6] = {
			vector3(1.0f, 0.0f, 0.0f),
			vector3(-1.0f, 0.0f, 0.0f),
			vector3(0.0f, 1.0f, 0.0f),
			vector3(0.0f, -1.0f, 0.0f),
			vector3(0.0f, 0.0f, 1.0f),
			vector3(0.0f, 0.0f, -1.0f)
		};
		vector3 upVectors[6] = {
			vector3(0.0f, -1.0f, 0.0f),
			vector3(0.0f, -1.0f, 0.0f),
			vector3(0.0f, 0.0f, 1.0f),
			vector3(0.0f, 0.0f, -1.0f),
			vector3(0.0f, -1.0f, 0.0f),
			vector3(0.0f, -1.0f, 0.0f)
		};

		Matrix4 currentCubeMapView = Matrix4();
		currentCubeMapView = currentCubeMapView.lookAt(modelOrigin, modelOrigin + targetVectors[face], upVectors[face]);

		// Render skybox
		shaderSkyBox->Use();
		view = camera.GetViewMatrix();
		view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "view"), 1, GL_FALSE, &currentCubeMapView.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderSkyBox->Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		SceneModels::RenderSkybox();

		shaderEnviromentObject->Use();
		model = Matrix4();
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "view"), 1, GL_FALSE, &currentCubeMapView.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "model"), 1, GL_FALSE, &model.data[0]);
		RenderScene(*shaderEnviromentObject, deltaTime);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO, set default framebuffer

	// Render scene as normal with the dynamic cube texture applied to the model
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderModel->Use();
	view = camera.GetViewMatrix();
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniform3f(glGetUniformLocation(shaderModel->Program, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel->Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);
	model = Matrix4();
	model = model.translate(vector3(modelOrigin.x, modelOrigin.y, modelOrigin.z));
	switch (currentModelID)
	{
	case 0:
		glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, &model.data[0]);
		modelSphere->Draw(*shaderModel);
		break;
	case 1:
		model = model.scale(vector3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, &model.data[0]);
		modelBunny->Draw(*shaderModel);
		break;
	case 2:
		model = model.scale(vector3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, &model.data[0]);
		modelUtahTeaPot->Draw(*shaderModel);
		break;
	}

	shaderEnviromentObject->Use();
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "model"), 1, GL_FALSE, &model.data[0]);
	RenderScene(*shaderEnviromentObject, deltaTime);

	// Skybox 
	shaderSkyBox->Use();
	view = camera.GetViewMatrix();
	view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderSkyBox->Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
	SceneModels::RenderSkybox();
}

void CubeMapDemo::RenderScene(Shader &shader, GLfloat deltaTime)
{
	shader.Use();

	Matrix4 model = Matrix4();
	Matrix4 scale = Matrix4();
	Matrix4 rotate = Matrix4();
	Matrix4 translate = Matrix4();
	translate = translate.translate(redPos);
	scale = scale.scale(vector3(1.0f, 1.0f, 4.0f));
	model = scale * rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, redTex);
	SceneModels::RenderCube();
	model = Matrix4();
	translate = translate.translate(bluePos);
	scale = scale.scale(vector3(1.5f, 1.5f, 2.5f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(45.0f));
	model = scale * rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, blueTex);
	SceneModels::RenderCube();
	model = Matrix4();
	translate = translate.translate(yellowPos);
	scale = scale.scale(vector3(2.0f, 3.0f, 1.0f));
	model = scale * rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, yellowTex);
	SceneModels::RenderCube();
	model = Matrix4();
	translate = translate.translate(greenPos);
	scale = scale.scale(vector3(2.0f, 2.0f, 2.0f));
	rotate = rotate.rotate(MathHelper::DegressToRadians(45.0f), vector3(1.0f, 0.0f, 1.0f));
	model = scale * rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, greenTex);
	SceneModels::RenderCube();
	model = Matrix4();
	translate = translate.translate(pinkPos);
	scale = scale.scale(vector3(2.5f, 2.5f, 1.5f));
	rotate = rotate.rotate(MathHelper::DegressToRadians(70.0f), vector3(1.0f, 0.0f, 0.4f));
	model = scale * rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, pinkTex);
	SceneModels::RenderCube();
	glBindTexture(GL_TEXTURE_2D, 0);
}