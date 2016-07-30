#include "CubeMapDemo.h"

/*
TODO(Darren):
	- Both reflection and refraction can be choosen in ImGui
*/

CubeMapDemo::CubeMapDemo()
	: initalizeScene(true)
{

}

CubeMapDemo::~CubeMapDemo()
{
	// TODO(Darren): Delete the buffers when switching scenes.
}

/*
	Need to render cubes with textures.
	Then create a cube map of the scene with the cubes not the utah teapot.
	Use a different cube map for final scene.
*/

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

		//shaderCubeMap.Compile("Shaders/CubeMapDemo/dynamicCubeMap.vert", "Shaders/CubeMapDemo/dynamicCubeMap.frag");
		//"Shaders/CubeMapDemo/dynamicCubeMap.gs");
		//modelCubes.Compile("Object.vert", "Object.frag");

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

		// Create dynamic cubemap

		//////////////////////////////
		// CREATE EMPTY CUBEMAP //////
		//////////////////////////////
		glGenTextures(1, &texCube);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// allocate space for each side of the cube map
		for (GLuint i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size,
				size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		/////////////////////////////////////////
		// CREATE FRAMBUFFER ////////////////////
		/////////////////////////////////////////
		glGenFramebuffers(1, &fb); // generate new frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb); // and bind it as active framebuffer
		glGenRenderbuffers(1, &drb); // generate new render buffer for depth buffer,
		glBindRenderbuffer(GL_RENDERBUFFER, drb);// bind it as renderbuffer and set parameters
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
		// Attach one of the faces of the cubemap texture to current framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, texCube, 0);
		// Attach depth buffer to framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, drb);
		// set current draw buffer
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);

		// attach only the +X cubemap texture (for completeness)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texCube, 0);

		//Check if current configuration of framebuffer is correct
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			break;
		default:
			std::cout << "bad framebuffer!" << std::endl;
			break;
		}

		// set default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		initalizeScene = false;
	}
}

// TODO(Darren): Reset up the buffers when the screen is resised
void CubeMapDemo::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	/*
		*** Creating a dynamic cube map. ***
	*/
	// 1. Create depth cubemap transformation matrices.
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 1000.0f;
	glm::mat4 projection_ = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
	glm::mat4 shadowProj = projection_;

	Matrix4 cubemapProjection = cubemapProjection.perspectiveProjection(camera.zoom, 1.0f, 1.0f, 500.0f);
	std::vector<Matrix4> cubemapTransforms;
	Matrix4 lookAt;
	cubemapTransforms.push_back(cubemapProjection * lookAt.lookAt(vector3(), vector3(1.0, 0.0, 0.0), vector3(0.0, -1.0, 0.0)));
	cubemapTransforms.push_back(cubemapProjection *	lookAt.lookAt(vector3(), vector3(-1.0, 0.0, 0.0), vector3(0.0, -1.0, 0.0)));
	cubemapTransforms.push_back(cubemapProjection * lookAt.lookAt(vector3(), vector3(0.0, 1.0, 0.0), vector3(0.0, 0.0, 1.0)));
	cubemapTransforms.push_back(cubemapProjection * lookAt.lookAt(vector3(), vector3(0.0, -1.0, 0.0), vector3(0.0, 0.0, -1.0)));
	cubemapTransforms.push_back(cubemapProjection * lookAt.lookAt(vector3(), vector3(0.0, 0.0, 1.0), vector3(0.0, -1.0, 0.0)));
	cubemapTransforms.push_back(cubemapProjection * lookAt.lookAt(vector3(), vector3(0.0, 0.0, -1.0), vector3(0.0, -1.0, 0.0)));

	// 2. Render scene to cubemap
	//////////////////////////////////
	// RENDER SCENE TO CUBE TEXTURE //
	//////////////////////////////////
	glViewport(0, 0, size, size); // set size of the viewport as size of cube map
	Matrix4 view;
	Matrix4 projection;
	Matrix4 model;
	projection = projection.perspectiveProjection(90.0f, 1.0f, 0.1f, 1000.0f);
	// HERE: set view matrix, look in positive direction of X axis
	glBindFramebuffer(GL_FRAMEBUFFER, fb); // bind FBO to render to the texture
	//glClearDepth(2.0f);       // 1/w depth buffer: depth = 0 means infinity
	//glDepthFunc(GL_GREATER);  // 1/w depth buffer: closer objects have greater depth
	// render to positive X face
	// bind cube map face to current framebuffer as render target
	for (GLuint face = 0; face < 6; ++face)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texCube, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		view = cubemapTransforms.at(face);

		glm::vec3 targets[6] = {
			glm::vec3(+1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, +1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, +1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f)
		};
		glm::vec3 ups[6] = {
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)
		};

		glm::mat4 v = glm::lookAt(modelOrigin, modelOrigin + targets[face], ups[face]);

		// skybox 
		shaderSkyBox->Use();
		view = camera.GetViewMatrix();
		view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "view"), 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderSkyBox->Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		SceneModels::RenderSkybox();

		shaderEnviromentObject->Use();
		model = Matrix4();
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "view"), 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
		glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "model"), 1, GL_FALSE, model.data);
		RenderScene(*shaderEnviromentObject);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind FBO, set default framebuffer

	// 3. Render scene as normal.
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderModel->Use();
	view = camera.GetViewMatrix();
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "projection"), 1, GL_FALSE, projection.data);
	glUniform3f(glGetUniformLocation(shaderModel->Program, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel->Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);	// texCube
	model = Matrix4();
	model = model.translate(vector3(modelOrigin.x, modelOrigin.y, modelOrigin.z));
	switch (currentModelID)
	{
	case 0:
		glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, model.data);
		modelSphere->Draw(*shaderModel);
		break;
	case 1:
		model = model.scale(vector3(0.1f, 0.1f, 0.1f));
		//glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, model.data);
		modelBunny->Draw(*shaderModel);
		break;
	case 2:
		model = model.scale(vector3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(shaderModel->Program, "model"), 1, GL_FALSE, model.data);
		modelUtahTeaPot->Draw(*shaderModel);
		break;
	}

	shaderEnviromentObject->Use();
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject->Program, "model"), 1, GL_FALSE, model.data);
	RenderScene(*shaderEnviromentObject);

	// skybox 
	shaderSkyBox->Use();
	view = camera.GetViewMatrix();
	view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox->Program, "projection"), 1, GL_FALSE, projection.data);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderSkyBox->Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
	SceneModels::RenderSkybox();

	// TODO(Darren): I will need to figure out when to delete buffers.
	// NOTE(Darren): Really need to do this!!!
}

void CubeMapDemo::RenderScene(Shader &shader)
{
	shader.Use();
	Matrix4 model = Matrix4();
	model = model.translate(vector3(15.0f, -3.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, redTex);
	SceneModels::RenderCube();

	vector3 translatePos;
	if (moveLights)
	{
		translatePos.x += sin(glfwGetTime()) * 4.5f;
		//translatePos.y += sin(glfwGetTime()) * 4.5f;
		translatePos.z += cos(glfwGetTime()) * 4.5f;
	}

	model = Matrix4();
	model = model.translate(vector3(4.0f, 5.0f, 1.0f) + translatePos);
	model = model.scale(vector3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, blueTex);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(-5.0f, -1.0f, 0.0f) + translatePos);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, yellowTex);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(-4.5f, 1.0f, 1.5f) + translatePos);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, greenTex);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(3.5f, 2.0f, 1.0f) + translatePos);
	model = model.rotate(60.0f, vector3(1.0f, 0.0f, 1.0f).normalise());
	model = model.scale(vector3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, pinkTex);
	SceneModels::RenderCube();
}