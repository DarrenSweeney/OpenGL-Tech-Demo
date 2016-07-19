#ifndef MODEL_LOADING_DEMO_H
#define MODEL_LOADING_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include <vector>
#include "camera.h"
#include "Math\vector3.h"
#include "Math\matrix4.h"

class ModelLoadingDemo
{
public:
	ModelLoadingDemo();
	~ModelLoadingDemo();

	bool showNormals, showCubemap;
	bool initalizeScene;

	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint cubeMapTextureID;
	bool normalMapping = true, inTangentSpace = true;
	vector3 lightPosition;
	Shader *shaderModelLoading, *shaderNormal, *shaderSkybox;
	Model *modelCyborg;
};

#endif