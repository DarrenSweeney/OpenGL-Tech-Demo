#ifndef SCENE_MODELS_H
#define SCENE_MODELS_H

#include <GL\gl3w.h>
#include "Math\vector3.h"
#include "Math\vector2.h"

class SceneModels
{
public:
	SceneModels();

	static void RenderQuad();
	static void RenderParallaxQuad(GLfloat pos = 1.0f, GLfloat texCoords = 1.0f);
	static void RenderCube(GLfloat xScale = 1.0f, GLfloat yScale = 1.0f, GLfloat zScale = 1.0f);
	static void RenderPlane(GLfloat pos = 1.0f, GLfloat texCoord = 1.0f);
	static void RenderSkybox();
};

#endif