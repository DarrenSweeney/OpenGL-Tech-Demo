#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <iostream>
#include <GL\gl3w.h>
#include <GLFW\glfw3.h>
#include <GL\glext.h>
#include <SOIL\SOIL.h>
#include <vector>

class ResourceManager
{
public:
	static GLuint LoadTexture(GLchar *path, GLboolean gammaCorrection = true,
		GLboolean alpha = false, GLboolean anisotropicFilter = true);

	static GLuint LoadCubeMap(std::vector<const GLchar*> faces);
};

#endif