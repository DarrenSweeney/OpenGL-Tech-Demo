#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <SOIL\SOIL.h>

#include "mesh.h"

GLuint TextureFromFile(const char *path, string directory);

class Model
{
public:
	Model();
	void LoadModel(GLchar *path);
	void Draw(Shader shader);

private:
	// Model Data
	vector<Mesh> meshes;
	string directory;
	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Texture> textures_loaded;
	// Model Functions
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};
#endif