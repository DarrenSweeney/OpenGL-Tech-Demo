#ifndef CUBE_MAP_DEMO_H
#define CUBE_MAP_DEMO_H

#include <vector>
#include "camera.h"
#include <SOIL/SOIL.h>
#include "Math/vector3.h"
#include "shader.h"
#include "model.h"

class CubeMapDemo
{
public:
	CubeMapDemo();
	~CubeMapDemo();
	void InitalizeScene();
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint skyboxVAO, skyboxVBO;
	std::vector<const GLchar*> faces;
	GLuint cubeMapTexture;
	Shader shaderModel, shaderSkyBox;
	Model modelUtahTeaPot;

	GLuint LoadCubeMap(std::vector<const GLchar*> faces);
};

#endif