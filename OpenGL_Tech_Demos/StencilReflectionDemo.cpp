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
	
	//glColorMask(0, 0, 0, 0);

	//glEnable(GL_STENCIL_TEST);					// Enable Stencil Buffer For "marking" The Floor
	//glStencilFunc(GL_ALWAYS, 1, 1);				// Always Passes, 1 Bit Plane, 1 As Mask
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// We Set The Stencil Buffer To 1 Where We Draw Any Polygon
	//											// Keep If Test Fails, Keep If Test Passes But Buffer Test Fails
	//											// Replace If Test Passes
	//glDisable(GL_DEPTH_TEST);                   // Disable Depth Testing
	////DrawFloor();								// Draw The Floor (Draws To The Stencil Buffer)
	//											// We Only Want To Mark It In The Stencil Buffer
	//// Draw Floor.
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0xFF);
	//glDepthMask(GL_FALSE);
	//glClear(GL_STENCIL_BUFFER_BIT);

	//glBindTexture(GL_TEXTURE_2D, planeTexture);
	//model = Matrix4();
	//glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	//SceneModels::RenderPlane();

	//glEnable(GL_DEPTH_TEST);                  // Enable Depth Testing
	//glColorMask(1, 1, 1, 1);                  // Set Color Mask to TRUE, TRUE, TRUE, TRUE
	//glStencilFunc(GL_EQUAL, 1, 1);            // We Draw Only Where The Stencil Is 1
	//										  // (I.E. Where The Floor Was Drawn)
	//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);   // Don't Change The Stencil Buffer

	
}