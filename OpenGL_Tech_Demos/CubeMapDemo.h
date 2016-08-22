#ifndef CUBE_MAP_DEMO_H
#define CUBE_MAP_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include <vector>
#include "camera.h"
#include "Math/vector3.h"

class CubeMapDemo
{
public:
	CubeMapDemo();
	~CubeMapDemo();

	bool initalizeScene;
	bool animateScene;
	int currentModelID;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, GLfloat deltaTime);

private:
	std::vector<const GLchar*> faces;
	GLuint cubeMapTextureID;
	GLuint dynamicCubeTex;
	GLuint framebuffer, depthRenderbuffer;
	const int cubeMapSize = 1024; // Size of the cube map texture image, width and height
	Shader *shaderModel, *shaderSkyBox, *shaderEnviromentObject;
	Model *modelUtahTeaPot, *modelSphere, *modelBunny;
	GLuint redTex, blueTex, yellowTex, greenTex, pinkTex;
	vector3 modelOrigin, bluePos, yellowPos, greenPos, pinkPos, redPos;
	Shader *shaderCubeMap;
	Shader *modelCubes;
	GLuint woodTextureID;

	void RenderScene(Shader &shader, GLfloat deltaTime);
};

#endif