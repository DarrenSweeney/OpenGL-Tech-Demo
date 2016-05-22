#ifndef PARALLAX_MAPPING_DEMO_H
#define PARALLAX_MAPPING_DEMO_H

#include "Math\vector3.h"
#include "Math\vector2.h"
#include "Math\matrix4.h"
#include "camera.h"
#include "ParallaxMappingDemo.h"
#include "shader.h"
#include <SOIL\SOIL.h>

class ParallaxMappingDemo
{
public:
	ParallaxMappingDemo();
	~ParallaxMappingDemo();

	void Initalize(vector3 &cameraPos);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	GLboolean enableParallax;
	Shader shaderParallax;
	GLuint diffuseMap, normalMap, displacementMap;
	vector3 lightPos;
	GLuint quadVAO;
	GLuint quadVBO;

	GLuint LoadTexture(GLchar *path);
	void RenderQuad();
};

#endif