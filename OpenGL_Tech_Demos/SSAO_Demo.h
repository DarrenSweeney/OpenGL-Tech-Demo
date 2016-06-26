#ifndef SSAO_DEMO_H
#define SSAO_DEMO_H

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "Math\vector3.h"
// Necessary for generation of random floats (for sample kernel and noise texture)
#include <random>
#include <GL/glext.h>

class SSAO_Demo
{
public:
	SSAO_Demo();
	~SSAO_Demo();
	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool windowResized);

private:
	Shader shaderGeometryPass, shaderLightingPass;
	Shader shaderSSAO, shaderSSAOBlur;
	Model sceneModel;
	
	vector3 lightPos, lightColor;

	GLuint gBuffer;
	GLuint gPositionDepth, gNormal, gAlbedo;
	GLuint rboDepth;
	GLuint ssaoFBO, ssaoBlurFBO;
	GLuint ssaoColorBuffer, ssaoColorBufferBlur;
	std::uniform_real_distribution<GLfloat> randomFloats;
	std::default_random_engine generator;
	std::vector<vector3> ssaoKernel;
	std::vector<vector3> ssaoNoise;
	GLuint noiseTexture;

	GLuint texture;

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
	GLfloat Lerp(GLfloat a, GLfloat b, GLfloat f);
	GLuint quadVAO;
	GLuint quadVBO;
	void RenderQuad();
	// RenderCube() Renders a 1x1 3D cube in NDC.
	GLuint cubeVAO;
	GLuint cubeVBO;
	void RenderCube();
};

#endif