#ifndef MESH_H
#define MESH_H

#include <string>	
#include <vector>

#include <assimp\Importer.hpp>

#include "math/vector3.h"
#include "math/vector2.h"
#include "shader.h"

using namespace std;

struct Vertex
{
	vector3 Position;
	vector3 Normal;
	vector2 TexCoords;
	vector3 Tangents;
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
	// Render Data
	GLuint VAO;
	bool loadTangents;

	// Mesh Data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// Functions
	Mesh(vector<Vertex> &verticies, vector<GLuint> &indices, vector<Texture> &textures, bool loadTangent = false);
	void Draw(Shader &shader);

private:
	// Render Data
	GLuint VBO, EBO;
	// Functions
	void setupMesh();
};
#endif