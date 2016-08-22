#ifndef INSTANCING_DEMO_H
#define INSTANCING_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Math\matrix4.h"
#include "Math\MathHelper.h"

class InstancingDemo
{
public:
	InstancingDemo();
	~InstancingDemo();

	bool initalizeScene;

	void InitalizeScene();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Model *modelRock;
	Shader *shaderInstancing, *shaderDirtGround;
	Matrix4 *modelMatrices;
	const GLuint amount;	// The amount of rocks and grass quads to render.
	GLuint grassTextureID, groundTextureID;

	void SetUpBuffers(GLuint &vao, Matrix4 *matrices, GLuint vbo = NULL,
		int sizeOfVertices = NULL, GLfloat *vertices = NULL);
};

#endif