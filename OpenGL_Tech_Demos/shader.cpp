#include "shader.h"

Shader::Shader() {}

Shader::~Shader()
{
	// TODO(Darren): Delete the shader from a call instead.
	//glDeleteProgram(Program);
}

void Shader::InitShader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath)
{
	// Replace the vertex/fragment source code from filepath
	std::string vertexCode, fragmentCode, geometryCode;
	std::ifstream vShaderFile, fShaderFile, gShaderFile;

	// Ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	if (geometryPath != NULL)
		gShaderFile.exceptions(std::ifstream::badbit);

	try
	{
		// Open file.
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		if (geometryPath != NULL)
			gShaderFile.open(geometryPath);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		// Read file's buffer contents into steam
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		if (geometryPath != NULL)
			gShaderStream << gShaderFile.rdbuf();
		// Cose file handlers
		vShaderFile.close();
		fShaderFile.close();
		if (geometryPath != NULL)
			gShaderFile.close();

		// Convert stream in GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometryPath != NULL)
			geometryCode = gShaderStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = (geometryPath != NULL ? geometryCode.c_str() : NULL);

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
	if (geometryPath != NULL)
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
	if (geometryPath != NULL)
		glAttachShader(this->Program, geometry);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::Program::LINKING_FAILED" << std::endl;
	}

	// Delete the shaders as they are linked into Program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != NULL)
		glDeleteShader(geometry);
}

void Shader::Use()
{
	glUseProgram(this->Program);
}