#ifndef STENCIL_REFLECTIONS_H
#define STENCIL_REFLECTIONS_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "Math\MathHelper.h"
#include "model.h"
#include <SOIL/SOIL.h>
#include "camera.h"
#include "shader.h"
#include <GL/glext.h>

class StencilReflectionDemo
{
public:
	StencilReflectionDemo();
	~StencilReflectionDemo();

	bool initalizeScene;
	bool planeTextureOff;

	void InitalizeScene();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint cubeTextureID, planeTextureID;
	Model *sceneModel;
	Shader *shaderObject, *shaderLighting;
};

#endif