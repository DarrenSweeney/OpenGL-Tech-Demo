#include "mesh.h"

Mesh::Mesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures, bool loadTangents)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->loadTangents = loadTangents;
	this->setupMesh();// Now that we have all the required data, set the vertex buffers and its attribute pointers.
}

//Initializes all the buffer objects/arrays. To setup the appropriate buffers though and specify the vertex shader layout via vertex attribute pointers
void Mesh::setupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	/* A great thing about structs is that their memory layout is sequential for all its items. The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which again translates to 3/2 floats which translates to a byte array.*/
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	//Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Vertex Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal)); //a preprocessor directive called offsetof(s,m) that takes as its first argument a struct and as its second argument a variable name of the struct. The macro returns the byte offset of that variable from the start of the struct. 
	glEnableVertexAttribArray(1);
	//Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);
	//Tangents
	if (loadTangents)
	{
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangents));
		glEnableVertexAttribArray(3);
	}


	glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader)// Render the mesh
{
	// Bind appropriate textures
	GLuint diffuseNr = 1, specularNr = 1, reflectionNr = 1, normalNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)
		std::stringstream ss;
		std::string number, name = this->textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; //transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; //transfer GLuint to stream
		else if (name == "texture_reflection") // We'll now also need to add the code to set and bind to reflection textures
			ss << reflectionNr++; //transfer GLuint to stream
		else if (name == "texture_normal") // We'll now also need to add the code to set and bind to reflection textures
			ss << normalNr++;
		number = ss.str();
		glUniform1i(glGetUniformLocation(shader.Program, ("material." + name + number).c_str()), i);// Now set the sampler to the correct texture unit
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);// And finally bind the texture
	}
	glActiveTexture(GL_TEXTURE0);

	glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)

																				   //Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint k = 0; k < this->textures.size(); k++)
	{
		glActiveTexture(GL_TEXTURE0 + k);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

// Note(Darren): May implement instance draw for mesh.
//void Mesh::Draw_Ins(Shader &shader, GLuint amount)
//{
//	// Bind appropriate textures
//	GLuint diffuseNr = 1, specularNr = 1, reflectionNr = 1;
//	for (GLuint i = 0; i < this->textures.size(); i++)
//	{
//		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
//										  // Retrieve texture number (the N in diffuse_textureN)
//		std::stringstream ss;
//		std::string number, name = this->textures[i].type;
//		if (name == "texture_diffuse")
//			ss << diffuseNr++; //transfer GLuint to stream
//		else if (name == "texture_specular")
//			ss << specularNr++; //transfer GLuint to stream
//		else if (name == "texture_reflection") // We'll now also need to add the code to set and bind to reflection textures
//			ss << reflectionNr++; //transfer GLuint to stream
//		number = ss.str();
//		glUniform1i(glGetUniformLocation(shader.Program, ("material." + name + number).c_str()), i);// Now set the sampler to the correct texture unit
//		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);// And finally bind the texture
//	}
//	glActiveTexture(GL_TEXTURE0);
//
//	glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
//
//	//Draw mesh
//	glBindVertexArray(this->VAO);
//	glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0, amount);
//	glBindVertexArray(0);
//
//	// Always good practice to set everything back to defaults once configured.
//	for (GLuint k = 0; k < this->textures.size(); k++)
//	{
//		glActiveTexture(GL_TEXTURE0 + k);
//		glBindTexture(GL_TEXTURE_2D, 0);
//	}
//}
