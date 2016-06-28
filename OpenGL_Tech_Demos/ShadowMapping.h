#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

#include "ResourceManager.h"
#include "SceneModels.h"
#include "camera.h"
#include "shader.h"
#include "model.h"
#include "Math\vector3.h"
#include <SOIL\SOIL.h>
#include <GL/glext.h>

class ShadowMapping
{
public:
	ShadowMapping();
	~ShadowMapping();
	GLuint depthMap, depthMapFBO;
	bool moveLight, renderLight;

	void InitalizeScene();
	void UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	Model modelTree, modelTeapot, modelRock;
	Shader shaderDepth, shaderShadowMap;
	Shader shaderDebugQuad;
	Shader shaderLightBox;
	vector3 lightPosition;
	GLuint floorTexture, cubeTexture, teapotTexture;
	const GLuint ShadowWidth, ShadowHeight;

	void RenderScene(Shader &shader);

	GLuint GenerateMultiSampleTexture(GLuint samples);
};

#endif