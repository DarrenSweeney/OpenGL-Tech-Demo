#ifndef RESOURCE_MANAGER_H
#define RESORUCE_MANAGER_H

#include <iostream>
#include <GL\gl3w.h>
#include <GLFW\glfw3.h>
#include <GL\glext.h>
#include <SOIL\SOIL.h>

class ResourceManager
{
public:
	static GLuint LoadTexture(GLchar *path, GLboolean gammaCorrection = true,
		GLboolean alpha = false, GLboolean anisotropicFilter = true);
};

#endif