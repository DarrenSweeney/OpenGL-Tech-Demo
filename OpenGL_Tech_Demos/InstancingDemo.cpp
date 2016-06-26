#include "InstancingDemo.h"

InstancingDemo::InstancingDemo()
	: amount(3000)
{
	modelMatrices = new Matrix4[amount];
}

InstancingDemo::~InstancingDemo()
{
	delete[] modelMatrices;
}

// Possibly something like this?
struct GrassBlade
{
	GLuint grassVAO, grassVBO;

	GLfloat quadVertices[20] = {
		// Positions        // Texture Coords
		-1.0f,	1.0f, 0.0f,	0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		1.0f,	1.0f, 0.0f,	1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,	1.0f, 1.0f,
	};
}; GrassBlade grassBlade;

// TODO(Darren): Take this out.
GLfloat DegressToRads_(GLfloat degrees)
{
	const GLfloat PI = 3.141592;

	return degrees * (PI / 180.0f);
}

/*
	TODO(Darren): Have models of ruins scattered. (Broken walls, Columns). (Possible, maybe/maybe not).
*/
void InstancingDemo::InitalizeScene()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	shaderRock.InitShader("instance.vert", "grass.frag");
	shaderGrass.InitShader("instance.vert", "grass.frag");
	shaderDirtGround.InitShader("object.vert", "object.frag");

	modelRock.LoadModel("Resources/rock/rock.obj");

	grassTexture = LoadTexture("Resources/grass.png", true);
	groundTexture = LoadTexture("Resources/brickwall.jpg", false);

	srand(glfwGetTime());
	GLfloat radius = 5.0f;
	GLfloat offset = 25.0f;
	Matrix4 model = Matrix4();
	GLfloat scale = 0.0f;
	for (GLuint i = 0; i < amount; i++)
	{
		if (i % 2 == 0)
		{
			model = Matrix4();
			// 1. Translation: Displace along circle with 'radius' in range [-offset, offset]
			GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
			GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat x = sin(angle) * radius + displacement;
			displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat z = cos(angle) * radius + displacement;
			model = model.translate(vector3(x, 0.0f, z));
		}
		else
			model = model.rotate(DegressToRads_(90.0f), vector3(0.0f, 1.0f, 0.0f));

		//  4. Now add to list of matrices
		modelMatrices[i] = model;
	}

	SetUpBuffers(grassBlade.grassVAO, modelMatrices, grassBlade.grassVBO, sizeof(grassBlade.quadVertices), grassBlade.quadVertices);

	srand(glfwGetTime());
	for (GLuint i = 0; i < amount; i++)
	{
		model = Matrix4();
		// 1. Translation: Displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = model.translate(vector3(x, -1.0f, z));

		// 2. Scale: Scale between 0.05 and 0.25
		scale = (rand() % 20) / 100.0f + 0.05f;
		model = model.scale(vector3(scale, scale, scale));

		//  4. Now add to list of matrices
		modelMatrices[i] = model;
	}

	for (GLuint i = 0; i < modelRock.meshes.size(); i++)
	{
		SetUpBuffers(modelRock.meshes[i].VAO, modelMatrices);
	}
}

void InstancingDemo::SetUpBuffers(GLuint &vao, Matrix4 *matrices, GLuint vbo, int sizeOfVertices, GLfloat *vertices)
{
	GLuint buffer;
	glBindVertexArray(vao);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(Matrix4), &matrices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (sizeOfVertices != NULL && vertices != NULL)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	// Vertex Attributes
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(sizeof(vector4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(2 * sizeof(vector4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(3 * sizeof(vector4)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
}

void InstancingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.ControllerMovement();

	Matrix4 projection;
	projection = projection.perspectiveProjection(45.0f, screenWidth / screenHeight, 1.0f, 3000.0f);
	glm::mat4 projection_ = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 model;

	shaderGrass.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderGrass.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
	glUniformMatrix4fv(glGetUniformLocation(shaderGrass.Program, "view"), 1, GL_FALSE, &view.data[0]);

	glBindTexture(GL_TEXTURE_2D, grassTexture);
	{
		glBindVertexArray(grassBlade.grassVAO);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
		glBindVertexArray(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);	// NOTE(Darren): Could do this brace thing for OpenGL start and end calls more often?

	shaderRock.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderRock.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
	glUniformMatrix4fv(glGetUniformLocation(shaderRock.Program, "view"), 1, GL_FALSE, &view.data[0]);

	glBindTexture(GL_TEXTURE_2D, modelRock.textures_loaded[0].id);
	for (GLuint i = 0; i < modelRock.meshes.size(); i++)
	{
		glBindVertexArray(modelRock.meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, modelRock.meshes[i].vertices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Draw the ground.
	shaderDirtGround.Use();
	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.95f, 0.0f));
	model = model.scale(vector3(31.0f, 0.0f, 31.0f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround.Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderDirtGround.Program, "model"), 1, GL_FALSE, &model.data[0]);
	RenderPlane();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void InstancingDemo::RenderGrassInstanceQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		};
		// Setup plane VAO
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

void InstancingDemo::RenderPlane()
{
	if (planeVAO == 0)
	{
		GLfloat planeVertices[] = {
			// Positions         // Texture Coords
			1.0f, -0.5f, 1.0f, 25.0f, 0.0f,
			-1.0f, -0.5f, -1.0f, 0.0f, 25.0f,
			-1.0f, -0.5f, 1.0f, 0.0f, 0.0f,

			1.0f, -0.5f, 1.0f, 25.0f, 0.0f,
			1.0f, -0.5f, -1.0f, 25.0f, 25.0f,
			-1.0f, -0.5f, -1.0f, 0.0f, 25.0f
		};

		// Setup plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

GLuint InstancingDemo::LoadTexture(GLchar* path, GLboolean alpha)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}