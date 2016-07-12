#include "shader.h"

Shader::Shader() {}

Shader::~Shader()
{
	// TODO(Darren): Delete the shader from a call instead.
	// NOTE(Darren): Might do this in resource manager.
	//glDeleteProgram(Program);
}

void Shader::Compile(const GLchar *vShaderCode, const GLchar *fShaderCode, const GLchar *gShaderCode)
{
	// Compile Shader
	GLuint vertex, fragment, geometry;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMILATION_FAILED" << std::endl;
	}

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::shader::FRAGMENT::COMPILATION_FAILED\n" << std::endl;
	}

	// Geometry shader
	if (gShaderCode != NULL)
	{
		// Geometry shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		// Print compile errors if any
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::shader::GEOMETRY::COMPILATION_FAILED\n" << std::endl;
		}
	}

	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	if (gShaderCode != NULL)
		glAttachShader(this->Program, geometry);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		//std::cout << "ERROR::SHADER::Program::LINKING_FAILED:" << " " <<
		//	vertexPath << " " << fragmentPath << (geometryPath != NULL ? geometryCode.c_str() : " ") << std::endl;
	}
	else
	{
		//std::cout << "SHADER::Program::LINKING_SUCESS " << vertexPath << " " << fragmentPath << " " << (geometryPath != NULL ? geometryCode.c_str() : " ") << std::endl;
	}

	// Delete the shaders as they are linked into Program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (gShaderCode != NULL)
		glDeleteShader(geometry);
}

void Shader::Use()
{
	glUseProgram(this->Program);
}