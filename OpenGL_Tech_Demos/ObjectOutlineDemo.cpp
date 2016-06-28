#include "ObjectOutlineDemo.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


ObjectOutlineDemo::ObjectOutlineDemo()
{

}

ObjectOutlineDemo::~ObjectOutlineDemo()
{
	// TODO(Darren): Do things like delete shader.
}

void ObjectOutlineDemo::InitalizeScene()
{
	// Setup some OpenGL opitions
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST); )

	// Set up and compile our shaders
	shaderObject.InitShader("Shaders/ObjectOutlineDemo/StencilTesting.vert", "Shaders/ObjectOutlineDemo/StencilTesting.frag");
	shaderOutline.InitShader("Shaders/ObjectOutlineDemo/StencilTesting.vert", "Shaders/ObjectOutlineDemo/ObjectOutline.frag");

	// Load textures
	cubeTexture = ResourceManager::LoadTexture("Resources/container2.png");
	floorTexture = ResourceManager::LoadTexture("Resources/container.jpg");
}

void ObjectOutlineDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	glClear(GL_STENCIL_BUFFER_BIT);

	// Set uniforms
	shaderOutline.Use();
	Matrix4 model;
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glm::mat4 _projection = glm::perspective(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
	shaderObject.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	// Draw floor as normal, we only care about the containers. The floor should NOT fill the stencil buffer so we set its mask to 0x00
	glStencilMask(0x00);
	// Floor
	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	SceneModels::RenderPlane(3.0f, 2.0f);

	// == =============
	// 1st. Render pass, draw objects as normal, filling the stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	// Cubes
	model = Matrix4();
	model = model.translate(vector3(-1.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	SceneModels::RenderCube();

	// == =============
	// 2nd. Render pass, now draw slightly scaled versions of the objects, this time disabling stencil writing.
	// Because stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are now not drawn, thus only drawing 
	// the objects' size differences, making it look like borders.
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	shaderOutline.Use();
	GLfloat scale = 1.1;
	// Cubes
	model = Matrix4();
	model = model.translate(vector3(-1.0f, 0.0f, -1.0f));
	model = model.scale(vector3(scale, scale, scale));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.0f, 0.0f));
	model = model.scale(vector3(scale, scale, scale));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	SceneModels::RenderCube();
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
}