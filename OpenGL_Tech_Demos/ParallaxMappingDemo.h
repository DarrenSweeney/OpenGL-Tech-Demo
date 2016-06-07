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
	bool enableParallax = true;
	float heightScale = 0.05f;

	void Initalize(vector3 &cameraPos);
	void Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight);

private:
	vector3 lightPos;
	Shader shaderParallax, shaderLightBox;
	GLuint diffuseMap_SquareBricks, normalMap_SquareBricks, displacementMap_SquareBricks;
	GLuint diffuseMap_GreyStonewall, normalMap_GreyStonewall, displacementMap_GreyStonewall;
	GLuint diffuseMap_Pebbles, normalMap_Pebbles, displacementMap_Pebbles;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint cubeVAO, cubeVBO;

	GLuint LoadTexture(GLchar *path);
	void RenderCube();
	void RenderQuad();
};

#endif