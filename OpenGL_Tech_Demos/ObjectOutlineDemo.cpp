#include "ObjectOutlineDemo.h"

ObjectOutlineDemo::ObjectOutlineDemo()
	: initalizeScene(true)
{

}

ObjectOutlineDemo::~ObjectOutlineDemo()
{
	// TODO(Darren): Do things like delete shader.
}

void ObjectOutlineDemo::InitalizeScene()
{
	if (initalizeScene)
	{
		// Setup some OpenGL opitions
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Set up and compile our shaders
		shaderObject = ResourceManager::GetShader("StencilTesting");
		shaderOutline = ResourceManager::GetShader("ObjectOutline");
		shaderLighting = ResourceManager::GetShader("Lighting");
		shaderLightBox = ResourceManager::GetShader("LightBox");

		// Load textures
		cubeTextureID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-diffuse.jpg");
		floorTextureID = ResourceManager::LoadTexture("Resources/marble.jpg");

		// Load models
		modelEnemy = ResourceManager::GetModel("Cyborg");

		lightPositions[0] = vector3(0.0f, 0.5f, -5.0f);
		lightPositions[1] = vector3(0.3f, 4.0f, -10.0f);
		lightPositions[2] = vector3(-0.3f, 4.0f, 0.0f);
		lightPositions[3] = vector3(0.0f, 0.5f, 5.0f);

		shaderLighting->Use();
		glUniform1i(glGetUniformLocation(shaderLighting->Program, "material.diffuse"), 0);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "material.shininess"), 32.0f);

		// Directional Light
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		// Point light 1
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].position"), lightPositions[0].x, lightPositions[0].y, lightPositions[0].z);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[0].quadratic"), 0.032);
		// Point light 2
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].position"), lightPositions[1].x, lightPositions[1].y, lightPositions[1].z);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[1].quadratic"), 0.032);
		// Point light 3
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].position"), lightPositions[2].x, lightPositions[2].y, lightPositions[2].z);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[2].quadratic"), 0.032);
		// Point light 4
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].position"), lightPositions[3].x, lightPositions[3].y, lightPositions[3].z);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shaderLighting->Program, "pointLights[3].quadratic"), 0.032);

		initalizeScene = false;
	}
}

void ObjectOutlineDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4 model;
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	shaderOutline->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline->Program, "projection"), 1, GL_FALSE, &projection.data[0]);

	shaderLighting->Use();
	glUniform3f(glGetUniformLocation(shaderLighting->Program, "viewPos"), camera.position.x, camera.position.y, camera.position.z);
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "projection"), 1, GL_FALSE, &projection.data[0]);

	shaderLightBox->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	vector3 lightColor = vector3(1.0f, 1.0f, 1.0f);
	float lightColorData[] = { lightColor.x, lightColor.y, lightColor.z };
	glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);

	shaderObject->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderObject->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderObject->Program, "projection"), 1, GL_FALSE, &projection.data[0]);

	glStencilMask(0x00);

	if (renderLights)
	{
		shaderLightBox->Use();
		for (int i = 0; i < 4; i++)
		{
			model = Matrix4();
			model = model.translate(lightPositions[i]);
			model = model.scale(vector3(0.5f, 0.5f, 0.5f));

			glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, &model.data[0]);
			SceneModels::RenderCube();
		}
	}

	shaderLighting->Use();
	model = Matrix4();
	model = model.translate(vector3(-4.0f, 0.5f, -2.0f));
	model = model.scale(vector3(6.0f, 2.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "model"), 1, GL_FALSE, &model.data[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTextureID);
	SceneModels::RenderCube();
	model = Matrix4();
	model = model.translate(vector3(0.0f, 0.5f, 2.0f));
	model = model.scale(vector3(4.0f, 2.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, cubeTextureID);
	SceneModels::RenderCube();

	// Floor
	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.56f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "model"), 1, GL_FALSE, &model.data[0]);
	glBindTexture(GL_TEXTURE_2D, floorTextureID);
	SceneModels::RenderPlane(15.0f, 10.0f);
	glDepthMask(GL_TRUE);

	/*
		1st. Render pass, draw objects as normal, filling the stencil buffer
	*/
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	model = Matrix4();
	model = model.translate(vector3(-2.0f, -0.55f, -10.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelEnemy->Draw(*shaderLighting);

	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.55f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLighting->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelEnemy->Draw(*shaderLighting);

	/*
		2nd. Render pass, now draw slightly scaled versions of the objects, this time disabling stencil 
		writing. Because stencil buffer is now filled with several 1s. The parts of the buffer that 
		are 1 are now not drawn, thus only drawing the objects' size differences, making it look like borders.
	*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	shaderOutline->Use();

	model = Matrix4();
	model = model.translate(vector3(-2.0f, -0.63f, -10.0f));
	model = model.scale(vector3(1.03f, 1.03f, 1.03f));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelEnemy->Draw(*shaderOutline);

	model = Matrix4();
	model = model.translate(vector3(0.0f, -0.63f, 0.0f));
	model = model.scale(vector3(1.03f, 1.03f, 1.03f));
	glUniformMatrix4fv(glGetUniformLocation(shaderOutline->Program, "model"), 1, GL_FALSE, &model.data[0]);
	modelEnemy->Draw(*shaderOutline);

	glStencilMask(0xFF);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}