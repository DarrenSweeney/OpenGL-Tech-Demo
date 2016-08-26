#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "shader.h"
#include "model.h"
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
	static std::map<std::string, Model> Models;

	static void LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	static Shader* GetShader(std::string name);
	static void LoadModel(const string path, bool loadTangent, std::string name);
	static Model* GetModel(std::string name);
	static GLuint LoadTexture(GLchar *path, GLboolean alpha = false);
	static GLuint LoadCubeMap(std::vector<const GLchar*> faces);
	static void LoadAllDemoResources();
	// Properly de-allocates all loaded resources
	static void Clear();

private:
	ResourceManager();

	// Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath = NULL);
	static Model  loadModel(const string path, bool loadTangent);
};

#endif