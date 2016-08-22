#ifndef OMNIDIRECTION_SHADOW
#define OMNIDIRECTION_SHADOW

#include "ResourceManager.h"
#include "SceneModels.h"
#include "Math\vector3.h"
#include "Math\MathHelper.h"
#include "camera.h"
#include <vector>
#include <GL/glext.h>

class OmnidirectionalShadowDemo
{
public:
	OmnidirectionalShadowDemo();
	~OmnidirectionalShadowDemo();
	
	bool initalizeScene;
	bool moveLight, renderLight;

	void Initalize();
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	vector3 lightPosition;
	Shader *shaderPointShadows, *shaderPointShadowsDepth, *shaderLightBox;
	GLuint woodTextureID, wallTextureID;
	Model *modelPlatform, *modelColumn, *modelBunny;
	GLuint depthMapFBO, depthCubemap;
	const GLuint ShadowWidth, ShadowHeight;

	void RenderScene(Shader &shader);
};

#endif