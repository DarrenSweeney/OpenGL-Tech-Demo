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

struct GrassBlade
{
	GLuint grassVAO, grassVBO;

	// NOTE(Darren): Should this be static to initalize once?
	GLfloat quadVertices[20] = {
		// Positions			// Texture Coords
		-1.0f,	1.0f, 0.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		 1.0f,	1.0f, 0.0f,		1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f,
	};
}; GrassBlade grassBlade;

void InstancingDemo::InitalizeScene()
{
	glEnable(GL_DEPTH_TEST);

	shaderInstancing.Compile("Shaders/InstancingDemo/instance.vert", "Shaders/InstancingDemo/grass.frag");
	shaderDirtGround.Compile("Shaders/EnviromentObject.vert", "Shaders/EnviromentObject.frag");

	modelRock.LoadModel("Resources/rock/rock.obj");

	grassTexture = ResourceManager::LoadTexture("Resources/grass.png", false, true);
	groundTexture = ResourceManager::LoadTexture("Resources/brickwall.jpg", false, false);

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
			GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
			GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat x = sin(angle) * radius + displacement;
			displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
			GLfloat z = cos(angle) * radius + displacement;
			model = model.translate(vector3(x, 0.0f, z));
		}
		else
			model = model.rotate(MathHelper::DegressToRadians(90.0f), vector3(0.0f, 1.0f, 0.0f));

		modelMatrices[i] = model;
	}

	SetUpBuffers(grassBlade.grassVAO, modelMatrices, grassBlade.grassVBO, sizeof(grassBlade.quadVertices), grassBlade.quadVertices);

	srand(glfwGetTime());
	for (GLuint i = 0; i < amount; i++)
	{
		model = Matrix4();
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = model.translate(vector3(x, -1.0f, z));

		scale = (rand() % 20) / 100.0f + 0.05f;
		model = model.scale(vector3(scale, scale, scale));

		modelMatrices[i] = model;
	}

	for (GLuint i = 0; i < modelRock.meshes.size(); i++)
		SetUpBuffers(modelRock.meshes[i].VAO, modelMatrices);
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

	shaderInstancing.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderInstancing.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection_));
	glUniformMatrix4fv(glGetUniformLocation(shaderInstancing.Program, "view"), 1, GL_FALSE, &view.data[0]);

	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glBindVertexArray(grassBlade.grassVAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

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
	SceneModels::RenderPlane(1.0f, 25.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
}