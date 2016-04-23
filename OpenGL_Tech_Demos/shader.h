#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL\glew.h>

class Shader
{
public:
	// The program ID
	GLuint Program;

	Shader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath);
	void Use();
};

#endif