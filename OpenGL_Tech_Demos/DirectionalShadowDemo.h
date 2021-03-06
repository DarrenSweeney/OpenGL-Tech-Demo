#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "camera.h"
#include "Math\vector3.h"
#include <SOIL\SOIL.h>
#include <GL/glext.h>

class DirectionalShadowDemo
{
public:
	DirectionalShadowDemo();
	~DirectionalShadowDemo();

	bool initalizeScene;
	GLuint depthMap, depthMapFBO;
	bool moveLight, renderLight;

	void InitalizeScene();
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Model *modelTree, *modelTeapot, *modelRock;
	Shader *shaderDepth, *shaderShadowMap;
	Shader *shaderLightBox;
	vector3 lightPosition;
	GLuint floorTextureID, cubeTextureID, teapotTextureID;
	const GLuint ShadowWidth, ShadowHeight;

	void RenderScene(Shader &shader);
};

#endif