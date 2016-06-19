#ifndef STENCIL_REFLECTIONS_H
#define STENCIL_REFLECTIONS_H

#include <SOIL/SOIL.h>
#include "camera.h"
#include "shader.h"
#include <GL/glext.h>

class StencilReflectionDemo
{
public:
	StencilReflectionDemo();
	~StencilReflectionDemo();
	void InitalizeScene();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	// TODO(Darren): Replace cube and plane with chess piece and game board.
	GLuint cubeVAO, cubeVBO;
	GLuint planeVAO, planeVBO;
	GLuint cubeTexture, planeTexture;
	Shader shaderObject;

	GLuint LoadTexture(GLchar* path);
};

#endif