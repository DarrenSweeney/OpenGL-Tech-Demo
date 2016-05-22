#include "InstancingDemo.h"

InstancingDemo::InstancingDemo()
{
	modelMatrices = new Matrix4[amount];
}

InstancingDemo::~InstancingDemo()
{
	delete[] modelMatrices;
}

void InstancingDemo::InitalizeScene()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shaderPlanet.InitShader("planet.vert", "planet.frag");
	shaderRock.InitShader("instance.vert", "planet.frag");

	rock.LoadModel("Resources/rock/rock.obj");
	planet.LoadModel("Resources/planet/planet.obj");

	srand(glfwGetTime());
	GLfloat radius = 150.0f;
	GLfloat offset = 35.0f;
	for (GLuint i = 0; i < amount; i++)
	{
		Matrix4 model;
		// 1. Translation: Displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = -2.5f + displacement * 0.4f;// Y value has smaller displacement
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = model.translate(vector3(x, y, z));

		// 2. Scale: Scale between 0.05 and 0.25
		GLfloat scale = (rand() % 20) / 100.0f + 0.05f;
		model = model.scale(vector3(scale, scale, scale));

		// 3. Rotation: Add ramdom rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = model.rotate(rotAngle, vector3(0.4f, 0.6f, 0.8f));

		//  4. Now add to list of matrices
		modelMatrices[i] = model;
	}

	for (GLuint i = 0; i < rock.meshes.size(); i++)
	{
		GLuint VAO = rock.meshes[i].VAO;
		// Vertex Buffer Object
		GLuint buffer;
		glBindVertexArray(VAO);
		// glGenVertexArrays(??????);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(Matrix4), &modelMatrices[0], GL_STATIC_DRAW);

		// Vertex Attributes
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(sizeof(vector4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(2 * sizeof(vector4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (GLvoid*)(3 * sizeof(vector4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}

void InstancingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.ControllerMovement();

	Matrix4 projection;
	projection = projection.perspectiveProjection(45.0f, screenWidth / screenHeight, 1.0f, 3000.0f);
	shaderPlanet.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderPlanet.Program, "projection"), 1, GL_FALSE, projection.data);
	shaderRock.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderRock.Program, "projection"), 1, GL_FALSE, projection.data);

	// Add the transformation matrices
	Matrix4 view;
	view = camera.GetViewMatrix();
	shaderPlanet.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderPlanet.Program, "view"), 1, GL_FALSE, view.data);
	shaderRock.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderRock.Program, "view"), 1, GL_FALSE, view.data);

	shaderPlanet.Use();
	Matrix4 model;
	model = model.translate(vector3(0.0f, -5.0f, 0.0f));
	model = model.scale(vector3(4.0f, 4.0f, 4.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderPlanet.Program, "model"), 1, GL_FALSE, model.data);
	planet.Draw(shaderPlanet);

	// Draw meteorites
	shaderRock.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderRock.Program, "model"), 1, GL_FALSE, model.data);
	glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
	for (GLuint i = 0; i < rock.meshes.size(); i++)
	{
		glBindVertexArray(rock.meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].vertices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}
}