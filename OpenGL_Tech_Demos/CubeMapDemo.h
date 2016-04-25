#ifndef CUBE_MAP_DEMO_H
#define CUBE_MAP_DEMO_H

#include <vector>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

class CubeMapDemo
{
public:
	CubeMapDemo();

private:
	GLuint LoadCubeMap(std::vector<const GLchar*> faces);
};

#endif