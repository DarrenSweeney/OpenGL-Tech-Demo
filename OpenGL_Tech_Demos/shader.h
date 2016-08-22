#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/gl3w.h>

class Shader
{
public:
	// The program ID
	GLuint Program;

	Shader();
	void Compile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath = NULL);
	void Use();
};

#endif