#ifndef OBJECT_OUTLINE_DEMO_H
#define OBJECT_OUTLINE_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Math\MathHelper.h"

class ObjectOutlineDemo
{
public:
	ObjectOutlineDemo();
	~ObjectOutlineDemo();

	bool initalizeScene;
	bool renderLights;

	void InitalizeScene();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Shader *shaderObject, *shaderOutline, *shaderLighting, *shaderLightBox;
	GLuint cubeTextureID, floorTextureID;
	Model *modelEnemy;
	vector3 lightPositions[4];
};

#endif