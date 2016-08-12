#ifndef CUBE_MAP_DEMO_H
#define CUBE_MAP_DEMO_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include <vector>
#include "camera.h"
#include <SOIL/SOIL.h>
#include "Math/vector3.h"
#include "shader.h"
#include "model.h"

// TODO(Darren): Remove GLM.
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CubeMapDemo
{
public:
	CubeMapDemo();
	~CubeMapDemo();

	bool initalizeScene;
	bool animateScene;
	int currentModelID;

	// Sort this out
	GLuint texture, texColorBuffer;
	GLuint texCube, fb, drb; // ids of cube texture, framebuffer and depth renderbuffer
	int size = 1024; // size of the cube map
	// Debug texture IDs
	GLuint redTex, blueTex, yellowTex, greenTex, pinkTex;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight, GLfloat deltaTime);

private:
	std::vector<const GLchar*> faces;
	GLuint cubeMapTextureID;
	Shader *shaderModel, *shaderSkyBox, *shaderEnviromentObject;
	Model *modelUtahTeaPot, *modelSphere, *modelBunny;

	glm::vec3 modelOrigin;
	vector3 bluePos;
	vector3 yellowPos;
	vector3 greenPos;
	vector3 pinkPos;
	vector3 redPos;

	Shader *shaderCubeMap;
	Shader *modelCubes;
	GLuint woodTextureID;

	void RenderScene(Shader &shader, GLfloat deltaTime);
};

#endif