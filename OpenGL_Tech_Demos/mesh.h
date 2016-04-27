#ifndef MESH_H
#define MESH_H

#include <string>	
#include <vector>

#include <assimp\Importer.hpp>

#include "math/vector3.h"
#include "math/vector2.h"
#include "shader.h"

using namespace std;

/*
A struct memory layout is sequential.
If we have a filled Vertex struct its memory layout would be equal to:

Vertex vertex;
vertex.Position = glm::vec3(0.2f, 0.4f, 0.6f);
vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
vertex.TexCoords = glm::vec2(1.0f, 0.0f);
// = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f];
*/

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

	// Functions
	Mesh(vector<Vertex> verticies, vector<GLuint> indices, vector<Texture> textures);
	void Draw(Shader shader);

private:
	// Render Data
	GLuint VAO, VBO, EBO;
	// Functions
	void setupMesh();
};
#endif