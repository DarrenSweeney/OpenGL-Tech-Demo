#include "StencilReflectionsDemo.h"

StencilReflectionDemo::StencilReflectionDemo()
{

}

StencilReflectionDemo::~StencilReflectionDemo()
{

}

void StencilReflectionDemo::InitalizeScene()
{
	// TODO(Darren): Need to comment on my purpose of calling OpenGL function calls
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	cubeTexture = ResourceManager::LoadTexture("Resources/brickwall.jpg");
	planeTexture = ResourceManager::LoadTexture("Resources/marble.jpg");

	shaderObject.InitShader("Shaders/EnviromentObject.vert", "Shaders/EnviromentObject.frag");
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
	shaderObject.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "projection"), 1, GL_FALSE, projection.data);

	GLuint uniColor = glGetUniformLocation(shaderObject.Program, "overrideColor");

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	// Draw Cubes
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = model.translate(vector3(-1.0f, 0.5f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderCube();

	// Draw Floor.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	glDepthMask(GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, planeTexture);
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	SceneModels::RenderPlane(3.0f, 2.0f);

	// Draw cube reflection.
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDepthMask(GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = Matrix4();
	model = model.translate(vector3(-1.0f, -0.5f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	//glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
	SceneModels::RenderCube();
	//glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	model = Matrix4();
	model = model.translate(vector3(2.0f, -0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	//glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
	SceneModels::RenderCube();
	//glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

	glDisable(GL_STENCIL_TEST);
}