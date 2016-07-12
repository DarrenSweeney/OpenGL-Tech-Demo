#ifndef MODEL_LOADING_DEMO_H
#define MODEL_LOADING_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include <vector>
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "Math\vector3.h"
#include "Math\matrix4.h"

class ModelLoadingDemo
{
public:
	ModelLoadingDemo();
	~ModelLoadingDemo();
	bool showNormals, showCubemap;

	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint cubeMapTexture;
	Model sceneModel;
	bool normalMapping = true, inTangentSpace = true;
	Matrix4 modelMatrix;
	vector3 lightPosition;
	// NOTE(Darren): Should i do it like this in other demos? ->Maybe not.
	GLuint shaderModelMatrix, shaderProjectionMatrix, shaderViewMatrix,
		shaderCameraPos, shaderNormalMapping, shaderInTangentSpace,
		shaderNormalProjection, shaderNormalView, shaderNormalModel;
};

#endif