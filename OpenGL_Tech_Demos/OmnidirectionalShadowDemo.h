#ifndef OMNIDIRECTION_SHADOW
#define OMNIDIRECTION_SHADOW

#include "shader.h"
#include "Math\vector3.h"
#include "camera.h"
#include <SOIL\SOIL.h>
#include <vector>

// TODO(Darren): Remove GLM.
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class OmnidirectionalShadowDemo
{
public:
	OmnidirectionalShadowDemo();
	~OmnidirectionalShadowDemo();
	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Shader shaderPointShadows;
	Shader shaderDepth;
	glm::vec3 lightPos;
	GLuint woodTexture, wallTexture;
	const GLuint ShadowWidth, ShadowHeight;
	GLuint depthMapFBO, depthCubemap;
	GLuint cubeVAO = 0;
	GLuint cubeVBO = 0;

	GLuint LoadTexture(GLchar *path);
	void RenderScene(Shader &shader);
	void RenderCube();
};

#endif