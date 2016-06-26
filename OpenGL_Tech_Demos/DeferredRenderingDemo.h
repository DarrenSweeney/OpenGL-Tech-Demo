#ifndef REFERRED_RENDERING_DEMO_H
#define REFERRED_RENDERING_DEMO_H

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Math\vector3.h"
#include <GL/glext.h>

class DeferredRenderingDemo
{
public:
	DeferredRenderingDemo();
	~DeferredRenderingDemo();
	bool moveLights, renderLights;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool windowResized);

private:
	Shader shaderGeometryPass, shaderLightingPass, shaderLightBox;
	Model sceneModel;
	std::vector<vector3> objectPositions;
	const GLuint NR_Lights;
	std::vector<vector3> lightPositions, lightColors;
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec;
	GLuint rboDepth;
	GLuint planeDiffuseTexture, planeSpecularTexture, objectDiffuseTexture, objectSpecularTexture;
	GLuint planeVAO, planeVBO;

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
	void RenderQuad();
	void RenderCube();
	void RenderPlane();
	GLuint LoadTexture(GLchar *path);
};


#endif