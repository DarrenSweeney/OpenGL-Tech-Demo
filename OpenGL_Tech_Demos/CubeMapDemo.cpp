#include "CubeMapDemo.h"

/*
Scene List:
- Both reflection and refraction can be choosen in ImGui
*/

CubeMapDemo::CubeMapDemo()
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
	// TODO(Darren): Add skybox to SceneModels
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
	glEnable(GL_CULL_FACE);

	shaderModel.InitShader("Shaders/CubeMapDemo/model.vert", "Shaders/CubeMapDemo/model.frag");
	shaderSkyBox.InitShader("Shaders/CubeMapDemo/skybox.vert", "Shaders/CubeMapDemo/skybox.frag");
	shaderEnviromentObject.InitShader("Shaders/EnviromentObject.vert", "Shaders/EnviromentObject.frag");

	shaderEnviromentObject.Use();
	// Set sampler.
	glUniform1i(glGetUniformLocation(shaderEnviromentObject.Program, "diffuseTexture"), 1);

	modelUtahTeaPot.LoadModel("Resources/utah-teapot.obj");

//	shaderCubeMap.InitShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
	//modelCubes.InitShader("Object.vert", "Object.frag");

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindVertexArray(0);

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

	cubeMapTexture = ResourceManager::LoadCubeMap(faces);
	woodTexture = ResourceManager::LoadTexture("Resources/brickwall.jpg");

	modelCubes.Use();
	glUniform1i(glGetUniformLocation(modelCubes.Program, "texture1"), 0);

	// TODO(Darren): Create the 6 color buffers, depth buffers and framebuffers.
	// ---

	// IN PROGRESS: Create a framebuffer of a single view and work from there.
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach it to the currently bound framebuffer object.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	shaderDebugQuad.InitShader("debugQuad.vert", "debugQuad.frag");
	shaderDebugQuad.Use();
	glUniform1i(glGetUniformLocation(shaderDebugQuad.Program, "screenTexture"), 5);
}

// TODO(Darren): Reset up the buffers when the screen is resised
void CubeMapDemo::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	/*
		*** Creating a dynamic cube map. ***
	*/
	// 1. Create depth cubemap transformation matrices.
	GLfloat aspect = (GLfloat)dy_skyboxResWidth / (GLfloat)dy_skyboxResHeight;
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(modelOrigin, modelOrigin + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	//// 2. Render scene to depth cubemap.
	//glViewport(0, 0, dy_skyboxResWidth, dy_skyboxResHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, dy_skyboxFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shaderCubeMap.Use();
	//for (GLuint i = 0; i < 6; ++i)
	//	glUniformMatrix4fv(glGetUniformLocation(shaderCubeMap.Program, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	//glUniform1f(glGetUniformLocation(shaderCubeMap.Program, "far_plane"), farPlane);
	//glUniform3fv(glGetUniformLocation(shaderCubeMap.Program, "lightPos"), 1, &modelOrigin[0]);
	//RenderScene(shaderCubeMap);	// ???
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);



	// 3. Render scene as normal.
	glViewport(0, 0, screenWidth, screenHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderModel.Use();
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
	Matrix4 model;
	model = model.translate(vector3(0.0f, 0.0f, -50.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "model"), 1, GL_FALSE, model.data);
	glUniform3f(glGetUniformLocation(shaderModel.Program, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	modelUtahTeaPot.Draw(shaderModel);

	shaderEnviromentObject.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "model"), 1, GL_FALSE, model.data);
	RenderScene(shaderEnviromentObject);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	/*shaderDebugQuad.Use();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	SceneModels::RenderQuad();
	glBindTexture(GL_TEXTURE_2D, 0);*/

	shaderModel.Use();
	view = camera.GetViewMatrix();
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
	
	model = model.translate(vector3(0.0f, 0.0f, -50.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "model"), 1, GL_FALSE, model.data);
	glUniform3f(glGetUniformLocation(shaderModel.Program, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	modelUtahTeaPot.Draw(shaderModel);

	shaderEnviromentObject.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderEnviromentObject.Program, "model"), 1, GL_FALSE, model.data);
	RenderScene(shaderEnviromentObject);

	// skybox 
	glDepthFunc(GL_LEQUAL);
	shaderSkyBox.Use();
	view = camera.GetViewMatrix();
	view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "projection"), 1, GL_FALSE, projection.data);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderSkyBox.Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

	// TODO(Darren): I will need to figure out when to delete buffers.
	// NOTE(Darren): Really need to do this!!!
}

void CubeMapDemo::RenderScene(Shader &shader)
{
	shader.Use();
	Matrix4 model = Matrix4();
	model = model.translate(vector3(4.0f, -3.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, 3.0f, 1.0f));
	model = model.scale(vector3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(-3.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(-1.5f, 1.0f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(-1.5f, 2.0f, -3.0f));
	model = model.rotate(60.0f, vector3(1.0f, 0.0f, 1.0f).normalise());
	model = model.scale(vector3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderCube();
}