#ifndef OBJECT_OUTLINE_DEMO_H
#define OBJECT_OUTLINE_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "camera.h"
#include <SOIL\SOIL.h>

class ObjectOutlineDemo
{
public:
	ObjectOutlineDemo();
	~ObjectOutlineDemo();

	void InitalizeScene();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Shader shaderObject, shaderOutline;
	GLuint cubeTexture, floorTexture;
};

#endif