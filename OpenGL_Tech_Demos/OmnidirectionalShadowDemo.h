#ifndef OMNIDIRECTION_SHADOW
#define OMNIDIRECTION_SHADOW

#include "ResourceManager.h"
#include "SceneModels.h"
#include "shader.h"
#include "model.h"
#include "Math\vector3.h"
#include "camera.h"
#include <SOIL\SOIL.h>
#include <vector>
#include <GL/glext.h>

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
	bool moveLight, renderLight;
	GLuint depthMapFBO, depthCubemap;

	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLuint skyboxVAO, skyboxVBO;
	Model modelPlatform, modelColumn, modelBunny;
	Shader shaderPointShadows, shaderDepth, shaderLightBox, shaderCubeMap;
	GLuint textureID;
	glm::vec3 lightPos;
	GLuint woodTexture, wallTexture;
	const GLuint ShadowWidth, ShadowHeight;
	//GLuint depthMapFBO, depthCubemap;

	void RenderScene(Shader &shader);
};

#endif