#ifndef REFERRED_RENDERING_DEMO_H
#define REFERRED_RENDERING_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "camera.h"
#include "Math\vector3.h"
#include <GL/glext.h>

class DeferredRenderingDemo
{
public:
	DeferredRenderingDemo();
	~DeferredRenderingDemo();

	bool initalizeScene;
	bool moveLights, renderLights;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool windowResized);

private:
	Shader *shaderGeometryPass, *shaderLightingPass, *shaderLightBox;
	Model *sceneModel;
	std::vector<vector3> objectPositions;
	const GLuint NR_Lights;
	std::vector<vector3> lightPositions, lightColors;
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec;
	GLuint rboDepth;
	GLuint planeDiffuseTextureID, planeSpecularTextureID, objectDiffuseTextureID, objectSpecularTextureID;

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
};


#endif