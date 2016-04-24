#include "mesh.h"

Mesh::Mesh(vector<Vertex> _verticies, vector<GLuint> _indices, vector<Texture> _textures)
{
	vertices = _verticies;
	indices = _indices;
	textures = _textures;
}

void Mesh::SetUpMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Vertex), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// Vertex Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
	// Bind appropiate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint reflectionNr = 1;

	for (GLuint i = 0; i < textures.size(); i++)
	{
		// Activate the proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		// Retrieve texture number (The N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = textures[i].type;

		// Transfer GLuint to stream and the read the 'N' number
		/*
			NOTE: When incrementing the diffuse, specular and refleciton they return 
			the variable to the stream and then increment after.
		*/
		if (name == "texture_diffuse")
			ss << diffuseNr++;
		else if (name == "texture_specular")
			ss << specularNr++;
		else if (name == "texure_reflection")
			ss << reflectionNr++;

		number = ss.str();

		glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glUniform1i(glGetUniformLocation(shader.Program, "material.shiness"), 16.0f);

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	for (GLuint i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}