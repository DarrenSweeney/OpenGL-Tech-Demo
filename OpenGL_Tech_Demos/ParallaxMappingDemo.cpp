#include "ParallaxMappingDemo.h"

ParallaxMappingDemo::ParallaxMappingDemo()
	: enableParallax(true), heightScale(0.05f), lightPos(0.5f, 1.0f, 70.4f), initalizeScene(true)
{
	
}

ParallaxMappingDemo::~ParallaxMappingDemo()
{
	// Square bricks
	if(diffuseMap_SquareBricksID)
		glDeleteTextures(1, &diffuseMap_SquareBricksID);
	if (normalMap_SquareBricksID)
		glDeleteTextures(1, &normalMap_SquareBricksID);
	if (displacementMap_SquareBricksID)
		glDeleteTextures(1, &displacementMap_SquareBricksID);

	// Grey stone wall
	if (diffuseMap_GreyStonewallID)
		glDeleteTextures(1, &diffuseMap_GreyStonewallID);
	if (normalMap_GreyStonewallID)
		glDeleteTextures(1, &normalMap_GreyStonewallID);
	if (displacementMap_GreyStonewallID)
		glDeleteTextures(1, &displacementMap_GreyStonewallID);

	// Pebbles
	if (diffuseMap_PebblesID)
		glDeleteTextures(1, &diffuseMap_PebblesID);
	if (normalMap_PebblesID)
		glDeleteTextures(1, &normalMap_PebblesID);
	if (displacementMap_PebblesID)
		glDeleteTextures(1, &displacementMap_PebblesID);
}

void ParallaxMappingDemo::Initalize(vector3 &cameraPos)
{
	if (initalizeScene)
	{
		cameraPos = vector3(0.0f, 0.0f, 60.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Setup and compile our shaders
		shaderParallax = ResourceManager::GetShader("ParallaxMapping");
		shaderLightBox = ResourceManager::GetShader("LightBox");

		// Load textures
		diffuseMap_SquareBricksID = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-diffuse.jpg");
		normalMap_SquareBricksID = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-normal.jpg");
		displacementMap_SquareBricksID = ResourceManager::LoadTexture("Resources/ParallaxTextures/SquareBricks/photosculpt-squarebricks-displace.jpg");

		diffuseMap_GreyStonewallID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-diffuse.jpg");
		normalMap_GreyStonewallID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-normal.jpg");
		displacementMap_GreyStonewallID = ResourceManager::LoadTexture("Resources/ParallaxTextures/GreyStonewall/photosculpt-graystonewall-displace.jpg");

		diffuseMap_PebblesID = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-diffuse.jpg");
		normalMap_PebblesID = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-normal.jpg");
		displacementMap_PebblesID = ResourceManager::LoadTexture("Resources/ParallaxTextures/Pebbles/photosculpt-pebbles-displace.jpg");

		// Set textures units
		shaderParallax->Use();
		glUniform1i(glGetUniformLocation(shaderParallax->Program, "diffuseMap"), 0);
		glUniform1i(glGetUniformLocation(shaderParallax->Program, "normalMap"), 1);
		glUniform1i(glGetUniformLocation(shaderParallax->Program, "depthMap"), 2);

		initalizeScene = false;
	}
}

void ParallaxMappingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	shaderParallax->Use();
	Matrix4 view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (float)screenWidth / (float)screenHeight, 1.0f, 600.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "view"), 1, GL_FALSE, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	
	if (!pauseLight)
	{
		//			 Radius		   Angle		 Origin
		lightPos.x = 65.0f * cos(glfwGetTime()) + 0.5f;
		lightPos.y = 65.0f * sin(glfwGetTime()) + 1.0f;
		lightPos.z = 65.0f * sin(glfwGetTime()) + 70.4f;
	}

	Matrix4 model = Matrix4();
	Matrix4 translate = Matrix4();
	Matrix4 rotate = Matrix4();
	Matrix4 scale = Matrix4();
	translate = translate.translate(vector3(0.0f, -80.0f, 58.0f));
	rotate = rotate.rotateX(MathHelper::DegressToRadians(-90.0f));
	model = rotate * translate;
	float lightData[] = { lightPos.x, lightPos.y, lightPos.z };
	float cameraPos[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]); 
	glUniform3fv(glGetUniformLocation(shaderParallax->Program, "lightPos"), 1, &lightData[0]);
	glUniform3fv(glGetUniformLocation(shaderParallax->Program, "viewPos"), 1, &cameraPos[0]);
	glUniform1f(glGetUniformLocation(shaderParallax->Program, "height_scale"), heightScale);
	glUniform1i(glGetUniformLocation(shaderParallax->Program, "parallax"), enableParallax);
	glUniform1i(glGetUniformLocation(shaderParallax->Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_PebblesID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_PebblesID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_PebblesID);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	model = Matrix4();
	translate = translate.translate(vector3(0.0f, -5.6f, 150.0f));
	rotate = rotate.rotateX(MathHelper::DegressToRadians(180.0f));
	model = rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]);
	glUniform1i(glGetUniformLocation(shaderParallax->Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_SquareBricksID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_SquareBricksID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_SquareBricksID);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	model = Matrix4();
	model = translate.translate(vector3(0.0f, -5.6f, -20.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	model = Matrix4();
	translate = translate.translate(vector3(80.0f, -5.6f, 50.0f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(-90.0f));
	model = rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	model = Matrix4();
	translate = translate.translate(vector3(-90.0f, -5.6f, 50.0f));
	rotate = rotate.rotateY(MathHelper::DegressToRadians(90.0f));
	model = rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	glUniform1i(glGetUniformLocation(shaderParallax->Program, "invertLayerShift"), false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_GreyStonewallID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap_GreyStonewallID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, displacementMap_GreyStonewallID);
	model = Matrix4();
	translate = translate.translate(vector3(0.0f, 80.0f, 58.0f));
	rotate = rotate.rotateX(MathHelper::DegressToRadians(90.0f));
	model = rotate * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderParallax->Program, "model"), 1, GL_FALSE, &model.data[0]);
	SceneModels::RenderParallaxQuad(100.0f, 3.0f);

	shaderLightBox->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "view"), 1, GL_FALSE, &view.data[0]);
	model = Matrix4();
	translate = translate.translate(lightPos);
	scale = scale.scale(vector3(6.55f, 6.55f, 6.55f));
	model = scale * translate;
	glUniformMatrix4fv(glGetUniformLocation(shaderLightBox->Program, "model"), 1, GL_FALSE, &model.data[0]);
	float lightColorData[] = {1.0f, 1.0f, 1.0f};
	glUniform3fv(glGetUniformLocation(shaderLightBox->Program, "lightColor"), 1, &lightColorData[0]);
	SceneModels::RenderCube();
}