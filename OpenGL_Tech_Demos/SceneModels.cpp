#include "SceneModels.h"

// Renders a 1x1 quad in NDC
void SceneModels::RenderQuad()
{
	GLuint quadVAO = 0, quadVBO = 0;

	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions			// Texture Coords
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// Renders a quad with normal, tangent and bitangent information.
void SceneModels::RenderParallaxQuad(GLfloat pos, GLfloat texCoords)
{
	GLuint quadVAO = 0, quadVBO = 0;

	if (quadVAO == 0)
	{
		// Positions
		vector3 pos1(-pos,  pos, 0.0f);
		vector3 pos2(-pos, -pos, 0.0f);
		vector3 pos3( pos, -pos, 0.0f);
		vector3 pos4( pos,  pos, 0.0f);

		// Texture coordinates
		vector2 uv1(0.0f, texCoords);
		vector2 uv2(0.0f, 0.0f);
		vector2 uv3(texCoords, 0.0f);
		vector2 uv4(texCoords, texCoords);

		// Normal vector
		vector3 nm(0.0f, 0.0f, 1.0f);

		// Calculating tangent/bitangent vectors of both triangels
		vector3 tangent1, bitangent1;
		vector3 tangent2, bitangent2;

		// Triangle 1
		vector3 edge1 = pos2 - pos1;
		vector3 edge2 = pos3 - pos1;
		vector2 deltaUV1 = uv2 - uv1;
		vector2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = tangent1.normalise();

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = bitangent1.normalise();

		// Triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = tangent2.normalise();


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = bitangent2.normalise();

		GLfloat quadVertices[] = {
			// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

GLuint cubeVAO = 0, cubeVBO = 0;
void SceneModels::RenderCube(GLfloat xScale, GLfloat yScale, GLfloat zScale)
{
	if (cubeVAO == 0)
	{
		GLfloat xyRatio = xScale / yScale;
		GLfloat xzRatio = xScale / zScale;
		GLfloat zyRatio = zScale / yScale;
		GLfloat yzRatio = yScale / zScale;

		GLfloat vertices[] = {
			//	  Positions				 Normal			Texture Coords
			// Back face
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,		// Bottom-left
			 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	xScale, yScale,		// top-right
			 0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	xScale, 0.0f,		// bottom-right         
			 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	xScale, yScale,		// top-right
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,		// bottom-left
			-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	0.0f, yScale,		// top-left
			// Front face
			-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,		// bottom-left
			 0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,		// bottom-right
			 0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,		// top-right
			 0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,		// top-right
			-0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,		// top-left
			-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,		// bottom-left
			// Left face
			-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,	yzRatio, 0.0f,		// top-right
			-0.5f,  0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,	yzRatio, zyRatio,	// top-left
			-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,	0.0f, zyRatio,  // bottom-left
			-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,	0.0f, zyRatio,	// bottom-left
			-0.5f, -0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,		// bottom-right
			-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,	yzRatio, 0.0f,		// top-right
			// Right face
			0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,		// top-left
			0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	0.0f, zyRatio,	// bottom-right
			0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	1.0f, zyRatio,	// top-right         
			0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	0.0f, zyRatio,  // bottom-right
			0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,		// top-left
			0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,		// bottom-left     
			// Bottom face
			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	0.0f, zyRatio,	// top-right
			 0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	1.0f, zyRatio,	// top-left
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,		// bottom-left
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,		// bottom-left
			-0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,		// bottom-right
			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	0.0f, zyRatio,	// top-right
			// Top face
			-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, zyRatio,	// top-left
			 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		// bottom-right
			 0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, zyRatio,	// top-right     
			 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		// bottom-right
			-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, zyRatio,	// top-left
			-0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f		// bottom-left        
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// Render Cube
	/*glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, endTexture);
	glDrawArrays(GL_TRIANGLES, 6, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glDrawArrays(GL_TRIANGLES, 12, 24);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);*/
}

GLuint planeVAO = 0, planeVBO = 0;
void SceneModels::RenderPlane(GLfloat pos, GLfloat texCoord)
{
	if (planeVAO == 0)
	{
		GLfloat planeVertices[] = {
			 // Positions		// Normals			// Texture Coords
			 pos, 0.0f,  pos,	0.0f, 1.0f, 0.0f,	texCoord, 0.0f,
			-pos, 0.0f, -pos,	0.0f, 1.0f, 0.0f,	0.0f, texCoord,
			-pos, 0.0f,  pos,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,

			 pos, 0.0f,  pos,	0.0f, 1.0f, 0.0f,	texCoord, 0.0f,
			 pos, 0.0f, -pos,	0.0f, 1.0f, 0.0f,	texCoord, texCoord,
			-pos, 0.0f, -pos,	0.0f, 1.0f, 0.0f,	0.0f, texCoord
		};

		// Setup plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}