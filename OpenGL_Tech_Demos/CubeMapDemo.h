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
	GLuint texture, texColorBuffer;

	void InitalizeScene(GLsizei screenWidth, GLsizei screenHeight);
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	std::vector<const GLchar*> faces;
	GLuint cubeMapTextureID;
	Shader *shaderModel, *shaderSkyBox, *shaderEnviromentObject;
	Model *modelUtahTeaPot;

	Shader *shaderCubeMap;
	Shader *modelCubes;
	GLuint dy_skyboxFBO, dy_skybox;
	glm::vec3 modelOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
	const GLuint dy_skyboxResWidth = 1024, dy_skyboxResHeight = 1024;
	GLuint woodTextureID;
	
	// Try agian, again here.
	GLuint fbo, rbo, framebuffer;
	Shader *shaderDebugQuad;

	void RenderScene(Shader &shader);
};

#endif