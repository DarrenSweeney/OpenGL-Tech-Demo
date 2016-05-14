#ifndef INSTANCING_DEMO_H
#define INSTANCING_DEMO_H

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Math\matrix4.h"

class InstancingDemo
{
public:
	InstancingDemo();
	~InstancingDemo();
	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void Update(Camera &camera);

private:
	Model planet, rock;
	Shader shaderPlanet, shaderRock;
	Matrix4 *modelMatrices;

	const GLuint amount = 10000;
};

#endif