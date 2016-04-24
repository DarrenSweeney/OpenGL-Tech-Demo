#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <SOIL\SOIL.h>
#include "mesh.h"

GLuint TextureFromFile(const GLchar *path, string directory);

class Model
{
public:
	Model(GLchar *path);
	void Draw(Shader shader);

private:
	// Model data
	vector<Mesh> meshes;
	string directory;
	// Stores all the textures loaded so far
	vector<Texture> textures_loaded;

	void LoadModel(string path);
	void ProcessNode(aiNode *node, const aiScene *aiScene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *aiScene);
	vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif