#ifndef STENCIL_REFLECTIONS_H
#define STENCIL_REFLECTIONS_H

#include "ResourceManager.h"
#include "SceneModels.h"
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
	GLuint cubeTexture, planeTexture;
	Shader shaderObject;
};

#endif