#ifndef OBJECT_OUTLINE_DEMO_H
#define OBJECT_OUTLINE_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "model.h"
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
	Shader shaderObject, shaderOutline, shaderLighting;
	GLuint cubeTexture, floorTexture;
	Model modelEnemy;

	//vector3 lightPositions[4];
	GLuint anArray[4];
};

#endif