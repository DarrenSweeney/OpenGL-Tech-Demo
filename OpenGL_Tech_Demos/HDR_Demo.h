#ifndef HRD_DEMO_H
#define HDR_DEMO_H

#include <vector>
#include "camera.h"
#include "shader.h"
#include <SOIL\SOIL.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class HDR_DEMO
{
public:
	HDR_DEMO();
	~HDR_DEMO();
	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, bool resized);

private:
	GLboolean hrd = true;
	GLfloat exposure = 0.05f;
	Shader shaderBlinnPhong, shaderHDR;
	GLuint woodTexture, endTexture;
	//std::vector<vector3> lightPositions;
	std::vector<glm::vec3> lightPositions;
	//std::vector<vector3> lightColors;
	std::vector<glm::vec3> lightColors;
	GLuint hdrFBO;
	GLuint colorBuffer;
	GLuint rboDepth;
	GLuint quadVAO, quadVBO;
	GLuint cubeVAO, cubeVBO;

	void SetupBuffers(GLsizei screenWidth, GLsizei screenHeight);
	GLuint LoadTexture(GLchar *path);
	void RenderQuad();
	void RenderCube();
};

#endif