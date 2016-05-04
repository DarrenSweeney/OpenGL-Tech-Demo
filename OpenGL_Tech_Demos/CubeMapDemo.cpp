#include "CubeMapDemo.h"

/*
	Scene List:
		- Both reflection and refraction can be choosen in ImGui
*/

CubeMapDemo::CubeMapDemo()
{
	
}

CubeMapDemo::~CubeMapDemo()
{
	// TODO(Darren): Delete the buffers when switching scenes.
}

void CubeMapDemo::InitalizeScene()
{
	#pragma region Skybox Vertices
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
#pragma endregion 

	// Setup some OpenGL opitions
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	// Note(Darren): Do i need this???

	// Cull the back face of the utah-tea pot.
	glEnable(GL_CULL_FACE);

	shaderModel.InitShader("Shaders/CubeMapDemo/model.vert", "Shaders/CubeMapDemo/model.frag");
	shaderSkyBox.InitShader("Shaders/CubeMapDemo/skybox.vert", "Shaders/CubeMapDemo/skybox.frag");
	modelUtahTeaPot.LoadModel("Resources/utah-teapot.obj");	//"Resources/utah-teapot.obj"

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindVertexArray(0);

	/*
		Order is based on the Layered Rendering specfic order.
		https://www.opengl.org/wiki/Cubemap_Texture
	*/
	faces.push_back("Resources/skybox/posx.jpg");
	faces.push_back("Resources/skybox/negx.jpg");
	faces.push_back("Resources/skybox/posy.jpg");
	faces.push_back("Resources/skybox/negy.jpg");
	faces.push_back("Resources/skybox/posz.jpg");
	faces.push_back("Resources/skybox/negz.jpg");

	//faces.push_back("Resources/skybox/right.jpg");
	//faces.push_back("Resources/skybox/left.jpg");
	//faces.push_back("Resources/skybox/top.jpg");
	//faces.push_back("Resources/skybox/bottom.jpg");
	//faces.push_back("Resources/skybox/back.jpg");
	//faces.push_back("Resources/skybox/front.jpg");

	cubeMapTexture = LoadCubeMap(faces);
}

// TODO(Darren): Will need to take out camera and put in main class.
void CubeMapDemo::UpdateScene(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	shaderModel.Use();
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
	Matrix4 model;
	model = model.translate(vector3(0.0f, 0.0f, -50.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "projection"), 1, GL_FALSE, projection.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderModel.Program, "model"), 1, GL_FALSE, model.data);
	glUniform3f(glGetUniformLocation(shaderModel.Program, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);

	// NOTE(Darren): why do you need to index each GL_TEXTURE?
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	modelUtahTeaPot.Draw(shaderModel);

	glDepthFunc(GL_LEQUAL);
	shaderSkyBox.Use();
	view = camera.GetViewMatrix();
	view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderSkyBox.Program, "projection"), 1, GL_FALSE, projection.data);

	// skybox 
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderModel.Program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

	// TODO(Darren): I will need to figure out when to delete buffers,
}

GLuint CubeMapDemo::LoadCubeMap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}