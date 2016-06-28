#include "ParallaxMappingDemo.h"

ParallaxMappingDemo::ParallaxMappingDemo()
	: enableParallax(true), lightPos(0.5f, 1.0f, 70.4f)
{
	
}

// NOTE(Darren): Bug, crash when camera is outside area.
ParallaxMappingDemo::~ParallaxMappingDemo()
{
	
}

/*
	Add one light in the middle of the scene. Have the light show color, 
	maybe allow change color.
*/
void ParallaxMappingDemo::Initalize(vector3 &cameraPos)
{
	cameraPos = vector3(0.0f, 0.0f, 60.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup and compile our shaders
	shaderParallax.InitShader("Shaders/ParallaxingMappingDemo/parallax_mapping.vert", "Shaders/ParallaxingMappingDemo/parallax_mapping.frag");
	shaderLightBox.InitShader("Shaders/light_box.vert", "Shaders/light_box.frag");

	// Load textures
	diffuseMap_SquareBricks = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-diffuse.jpg");
	normalMap_SquareBricks = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-normal.jpg");
	displacementMap_SquareBricks = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-displace.jpg");

	diffuseMap_GreyStonewall = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-diffuse.jpg");
	normalMap_GreyStonewall = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-normal.jpg");
	displacementMap_GreyStonewall = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-displace.jpg");

	diffuseMap_Pebbles = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-diffuse.jpg");
	normalMap_Pebbles = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-normal.jpg");
	displacementMap_Pebbles = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-displace.jpg");

	// Set textures units
	shaderParallax.Use();
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "depthMap"), 2);
}

void ParallaxMappingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	// Configure view/projection matrices
	shaderParallax.Use();
	Matrix4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 600.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//			 Radius		   Angle		 Origin
	lightPos.x = 65.0f * cos(glfwGetTime()) + 0.5f;
	lightPos.y = 65.0f * sin(glfwGetTime()) + 1.0f;
	lightPos.z = 65.0f * sin(glfwGetTime()) + 70.4f;

	// Render normal-mapped quad
	Matrix4 model = Matrix4();
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -80.0f, 58.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = model.translate(vector3(0.0f, 0.0f, 60.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	float lightData[] = { lightPos.x, lightPos.y, lightPos.z };
	glUniform3fv(glGetUniformLocation(shaderParallax.Program, "lightPos"), 1, &lightData[0]);
	float cameraPos[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderParallax.Program, "viewPos"), 1, &cameraPos[0]);
	glUniform1f(glGetUniformLocation(shaderParallax.Program, "height_scale"), heightScale);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "parallax"), enableParallax);
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_Pebbles);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_Pebbles);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_Pebbles);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -5.6f, 150.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform1i(glGetUniformLocation(shaderParallax.Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_SquareBricks);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_SquareBricks);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_SquareBricks);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -5.6f, -20.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(80.0f, -5.6f, 50.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-90.0f, -5.6f, 50.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	glUniform1i(glGetUniformLocation(shaderParallax.Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_GreyStonewall);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_GreyStonewall);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_GreyStonewall);
	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 80.0f, 58.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	shaderLightBox.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox.Program, "view"), 1, GL_FALSE, &view.data[0]);
	model = Matrix4();
	model = model.translate(lightPos);
	model = model.scale(vector3(5.05f, 5.05f, 5.05f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox.Program, "model"), 1, GL_FALSE, &model.data[0]);
	float lightColorData[] = {1.0f, 1.0f, 1.0f};
	glUniform3fv(glGetUniformLocation(shaderLightBox.Program, "lightColor"), 1, &lightColorData[0]);
	SceneModels::RenderCube();
}