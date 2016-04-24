#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <assimp\Importer.hpp>
#include "Math\vector2.h"
#include "Math\vector3.h"
#include "shader.h"

using namespace std;

struct Vertex
{
	vector3 Position;
	vector3 Normal;
	vector2 TexCoords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

class Mesh
{
public:
	// Mesh Data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> _verticies, vector<GLuint> _indices, vector<Texture> _textures);
	void Draw(Shader shader);

private:
	// Render Data
	GLuint VAO, VBO, EBO;

	void SetUpMesh();
};

#endif