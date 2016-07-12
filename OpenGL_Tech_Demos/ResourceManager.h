#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "shader.h"
#include <iostream>
#include <GL\gl3w.h>
#include <GLFW\glfw3.h>
#include <GL\glext.h>
#include <SOIL\SOIL.h>
#include <vector>
#include <map>
#include <string>

class ResourceManager
{
public:
	// Resource Storage 
	static std::map<std::string, Shader> Shaders;

	static Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	static Shader GetShader(std::string name);
	// Properly de-allocates all loaded resources
	static void Clear();

	static GLuint LoadTexture(GLchar *path, GLboolean gammaCorrection = true,
		GLboolean alpha = false, GLboolean anisotropicFilter = true);

	static GLuint LoadCubeMap(std::vector<const GLchar*> faces);

private:
	ResourceManager();

	// Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath = NULL);
};

#endif