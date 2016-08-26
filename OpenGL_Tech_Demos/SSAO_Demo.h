#ifndef SSAO_DEMO_H
#define SSAO_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "Math\vector3.h"
#include <random>
#include <GL/glext.h>

class SSAO_Demo
{
public:
	SSAO_Demo();
	~SSAO_Demo();

	// Parameters used to tweak the ssao uniform variables. Samples is defined as 64.
	int kernelSize;
	float radius;
	float noiseScale;

	bool initalizeScene;
	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool windowResized);

private:
	Shader *shaderGeometryPass, *shaderLightingPass;
	Shader *shaderSSAO, *shaderSSAOBlur;
	Model *modelSphere, *modelMonkey, *modelStatue;
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

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
	GLfloat Lerp(GLfloat a, GLfloat b, GLfloat f);
};

#endif