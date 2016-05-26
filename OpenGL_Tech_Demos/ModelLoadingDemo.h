#ifndef MODEL_LOADING_DEMO_H
#define MODEL_LOADING_DEMO_H

#include <vector>
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "Math\vector3.h"
#include "Math\matrix4.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ModelLoadingDemo
{
public:
	ModelLoadingDemo();
	~ModelLoadingDemo();

	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint skyboxVAO, skyboxVBO;
	std::vector<const GLchar*> faces;
	GLuint cubeMapTexture;
	Shader shaderModel, shaderSkyBox;
	Model sceneModel;
	bool normalMapping = true, inTangentSpace = true;
	Matrix4 modelMatrix;
	vector3 lightPosition;
	// NOTE(Darren): Should i do it like this in other demos?
	GLuint shaderModelMatrix, shaderProjectionMatrix, shaderViewMatrix,
		shaderCameraPos, shaderNormalMapping, shaderInTangentSpace;

	GLuint LoadCubeMap(std::vector<const GLchar*> faces);
};

#endif