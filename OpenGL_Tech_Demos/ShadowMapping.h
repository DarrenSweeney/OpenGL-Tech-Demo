#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

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
	// Shader shaderBunny;	// NOTE(Darren): Do i need this?
	Shader shaderDepth, shaderShadowMap;
	Shader shaderDebugQuad;
	Shader shaderLightBox;
	vector3 lightPosition;
	GLuint floorTexture, cubeTexture, teapotTexture;
	GLfloat fLargest;
	const GLuint ShadowWidth, ShadowHeight;
	GLuint planeVAO, planeVBO;
	GLuint cubeVAO, cubeVBO;
	GLuint quadVAO, quadVBO;

	void RenderScene(Shader &shader);
	void RenderQuad();
	void RenderCube();
	GLuint LoadTexture(GLchar *path, bool gammaCorrection);
	GLuint GenerateMultiSampleTexture(GLuint samples);
};

#endif