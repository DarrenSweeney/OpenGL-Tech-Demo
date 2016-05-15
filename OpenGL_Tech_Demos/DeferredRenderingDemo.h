#ifndef REFERRED_RENDERING_DEMO_H
#define REFERRED_RENDERING_DEMO_H

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Math\vector3.h"

class DeferredRenderingDemo
{
public:
	DeferredRenderingDemo();
	~DeferredRenderingDemo();
	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Shader shaderGeometryPass, shaderLightingPass, shaderLightBox;
	Model sceneModel;
	std::vector<vector3> objectPositions;
	const GLuint NR_Lights;
	std::vector<vector3> lightPositions, lightColors;
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec;
	GLuint rboDepth;

	void RenderQuad();
	void RenderCube();
	GLuint LoadTexture(GLchar *path);
};


#endif