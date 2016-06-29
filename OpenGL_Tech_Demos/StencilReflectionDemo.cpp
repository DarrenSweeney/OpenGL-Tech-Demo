#include "StencilReflectionsDemo.h"

StencilReflectionDemo::StencilReflectionDemo()
	: planeTextureOff(false)
{

}

StencilReflectionDemo::~StencilReflectionDemo()
{

}

/*
	Might make multi-color lights, need the render light positions and make them move, 
	to demonstate dynamic reflection. Set up scene models better, pillar resting on long cube
	and have a utah-teapot. May clean up initalising on the point light uniforms.
	Look into rendering stencil buffer to ImGui::Image();
*/
void StencilReflectionDemo::InitalizeScene()
{
	// TODO(Darren): Need to comment on my purpose of calling OpenGL function calls4
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

	cubeTexture = ResourceManager::LoadTexture("Resources/brickwall.jpg");
	planeTexture = ResourceManager::LoadTexture("Resources/marble.jpg");
    
	modelStatue.LoadModel("Resources/utah-teapot.obj");
	modelColumn.LoadModel("Resources/tree1b_lod0_2.obj");

	shaderObject.InitShader("Shaders/EnviromentObject.vert", "Shaders/EnviromentObject.frag");
	shaderLighting.InitShader("Shaders/StencilReflectionDemo/lighting.vert", "Shaders/StencilReflectionDemo/lighting.frag");

	shaderLighting.Use();
	glUniform1i(glGetUniformLocation(shaderLighting.Program, "material.diffuse"), 0);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "material.shininess"), 32.0f);

	// Directional Light
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

	vector3 lightPositions[] = {
		vector3(0.7f, 0.2f, 2.0f),
		vector3(2.3f, -3.3f, -4.0f),
		vector3(-4.0f, 2.0f, -12.0f),
		vector3(0.0f, 0.0f, -3.0f)
	};

	// Point light 1
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].position"), lightPositions[0].x, lightPositions[0].y, lightPositions[0].z);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[0].quadratic"), 0.032);
	// Point light 2
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].position"), lightPositions[1].x, lightPositions[1].y, lightPositions[1].z);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[1].quadratic"), 0.032);
	// Point light 3
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].position"), lightPositions[2].x, lightPositions[2].y, lightPositions[2].z);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[2].quadratic"), 0.032);
	// Point light 4
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].position"), lightPositions[3].x, lightPositions[3].y, lightPositions[3].z);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].linear"), 0.09);
	glUniform1f(glGetUniformLocation(shaderLighting.Program, "pointLights[3].quadratic"), 0.032);
}

void StencilReflectionDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	camera.ControllerMovement();

	Matrix4 model;
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

	shaderLighting.Use();
	glUniform1i(glGetUniformLocation(shaderLighting.Program, "reflection"), false);
	glUniform3f(glGetUniformLocation(shaderLighting.Program, "viewPos"), camera.position.x, camera.position.y, camera.position.z);
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "projection"), 1, GL_FALSE, &projection.data[0]);

	shaderObject.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "projection"), 1, GL_FALSE, projection.data);

	shaderLighting.Use();
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	// Draw Cubes
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = model.translate(vector3(-1.0f, 0.5f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();
    
    model = Matrix4();
	model = model.translate(vector3(0.0f, 0.5f, 0.0f));
	model = model.scale(vector3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	modelStatue.Draw(shaderLighting);

	//model = Matrix4();
	//model = model.translate(vector3(1.0f, 0.0f, 2.0f));
	//model = model.scale(vector3(0.02f, 0.02f, 0.02f));
	//glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	//modelColumn.Draw(shaderObject);

	// Draw Floor
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	glDepthMask(GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);

	shaderObject.Use();
	if (!planeTextureOff)
		glBindTexture(GL_TEXTURE_2D, planeTexture);
	else
		glBindTexture(GL_TEXTURE_2D, 0);
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderPlane(3.0f, 8.0f);

	// Draw cube reflection
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDepthMask(GL_TRUE);

	shaderLighting.Use();
	glUniform1i(glGetUniformLocation(shaderLighting.Program, "reflection"), true);

	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = Matrix4();
	model = model.translate(vector3(-1.0f, -0.5f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, -0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();

	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.5f, 0.0f));
	model = model.rotateX(MathHelper::DegressToRadians(180.0f));
	model = model.scale(vector3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting.Program, "model"), 1, GL_FALSE, model.data);
	modelStatue.Draw(shaderLighting);

	glDisable(GL_STENCIL_TEST);
}