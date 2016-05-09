#include "StencilReflectionsDemo.h"

StencilReflectionDemo::StencilReflectionDemo()
{

}

StencilReflectionDemo::~StencilReflectionDemo()
{

}

void StencilReflectionDemo::InitalizeScene()
{
	// TODO(Darren): Need to comment on my purpose of calling OpenGL function calls
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

#pragma region Object_Vertices
	// Set the object data (buffers, vertex attributes)
	GLfloat cubeVertices[] = {
		// Positions	          // Texture Coords
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
#pragma endregion

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
	planeVAO, planeVBO;
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

	cubeTexture = LoadTexture("Resources/grid.png");
	planeTexture = LoadTexture("Resources/grid.png");

	shaderObject.InitShader("Object.vert", "Object.frag");
}

void StencilReflectionDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	// Clear the color buffer
	glClearColor(0.4f, 0.4f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Draw objects
	//outlineShader.Use();
	Matrix4 model;
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	//glUniformMatrix4fv(glGetUniformLocation(outlineShader.Program, "view"), 1, GL_FALSE, view.data);
	//glUniformMatrix4fv(glGetUniformLocation(outlineShader.Program, "projection"), 1, GL_FALSE, projection.data);
	shaderObject.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "projection"), 1, GL_FALSE, projection.data);

	GLuint uniColor = glGetUniformLocation(shaderObject.Program, "overrideColor");

	// Cubes 
	glBindVertexArray(cubeVAO);
	// We omit the glActiveTexture part since TEXTURE0 is already the default active 
	// texture unit. (sampler used in fragment is set to 0 as well as default)	
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = model.translate(vector3(-1.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = Matrix4();
	model = model.translate(vector3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glEnable(GL_STENCIL_TEST);

	// Draw floor as normal. The floor should not fill the stencil buffer so we set it's mask to 0x00
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	// Floor
	glBindVertexArray(planeVAO);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	model = Matrix4();
	//model = model.translate(vector3(-3.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	glDepthMask(GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glUniform3f(uniColor, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, 66);
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(0);

	// 2nd. Render pass, draw slightly scaled versions of the objects, this time disabling stencil writing.
	// Because stencil buffer is now filled with several 1s. The parts of the buffer are 1 are now not draw, 
	// thus only drawing the object's size differences, making it look like borders.
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDepthMask(GL_TRUE);
	//shader.Use();
	Matrix4 reflection_Y = Matrix4(1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	// Cubes 
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = Matrix4();
	model = model.translate(vector3(-1.0f, -1.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	model = Matrix4();
	model = model.translate(vector3(2.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderObject.Program, "model"), 1, GL_FALSE, model.data);
	glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(0);

	glDisable(GL_STENCIL_TEST);

}

GLuint StencilReflectionDemo::LoadTexture(GLchar* path)
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