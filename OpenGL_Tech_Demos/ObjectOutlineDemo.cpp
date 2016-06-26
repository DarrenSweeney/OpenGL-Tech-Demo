#include "ObjectOutlineDemo.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


ObjectOutlineDemo::ObjectOutlineDemo()
{

}

ObjectOutlineDemo::~ObjectOutlineDemo()
{
	// TODO(Darren): Do things like delete shader.
}

void ObjectOutlineDemo::InitalizeScene()
{

	// Setup some OpenGL opitions
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST); )

	// Set up and compile our shaders
	shaderObject.InitShader("StencilTesting.vert", "StencilTesting.frag");
	shaderOutline.InitShader("StencilTesting.vert", "ObjectOutline.frag");

	// TODO(Darren): Need to create a resource manager to call ResourceManager::RenderCube(); Something like this.
	// Set the object data (buffers, vertex attributes)
	GLfloat cubeVertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	GLfloat planeVertices[] = {
		// Texture Coords: note we set these higher than 1 that together with GL_REPEAT 
		// as texture wrapping mode will cause the floor texture to repeat)
		// Positions         // Texture Coords 
		3.0f,  -0.5f,  3.0f,  2.0f, 0.0f,
		-3.0f, -0.5f,  3.0f,  0.0f, 0.0f,
		-3.0f, -0.5f, -3.0f,  0.0f, 2.0f,

		3.0f,  -0.5f,  3.0f,  2.0f, 0.0f,
		-3.0f, -0.5f, -3.0f,  0.0f, 2.0f,
		3.0f,  -0.5f, -3.0f,  2.0f, 2.0f
	};

	// Setup cube VAO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
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

	// Load textures
	cubeTexture = LoadTexture("Resources/container2.png");
	floorTexture = LoadTexture("Resources/container.jpg");
}

void ObjectOutlineDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	glClear(GL_STENCIL_BUFFER_BIT);

	// Set uniforms
	shaderOutline.Use();
	Matrix4 model;
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glm::mat4 _projection = glm::perspective(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
	shaderObject.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));


	// Draw floor as normal, we only care about the containers. The floor should NOT fill the stencil buffer so we set its mask to 0x00
	glStencilMask(0x00);
	// Floor
	glBindVertexArray(planeVAO);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	model = Matrix4();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// == =============
	// 1st. Render pass, draw objects as normal, filling the stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	// Cubes
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = model.translate(vector3(-1.0f, 0.1f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.1f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// == =============
	// 2nd. Render pass, now draw slightly scaled versions of the objects, this time disabling stencil writing.
	// Because stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are now not drawn, thus only drawing 
	// the objects' size differences, making it look like borders.
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	shaderOutline.Use();
	GLfloat scale = 1.1;
	// Cubes
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = Matrix4();
	model = model.translate(vector3(-1.0f, 0.1f, -1.0f));
	model = model.scale(vector3(scale, scale, scale));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.1f, 0.0f));
	model = model.scale(vector3(scale, scale, scale));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline.Program, "model"), 1, GL_FALSE, &model.data[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
}


// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint ObjectOutlineDemo::LoadTexture(GLchar* path)
{
	// Generate a texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	if (!image)
		std::cout << "ERROR:: Image was not loaded!" << std::endl;
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}