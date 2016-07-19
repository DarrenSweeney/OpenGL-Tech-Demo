#include "ModelLoadingDemo.h"

ModelLoadingDemo::ModelLoadingDemo()
	: lightPosition(0.0f, 0.0f, -5.0f), showNormals(false), showCubemap(false), initalizeScene(true)
{

}

ModelLoadingDemo::~ModelLoadingDemo()
{
	// TODO(Darren):
	// Dealocation? >>> glBindVertexArray(0);
	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//					glBindBuffer(GL_ARRAY_BUFFER, 0);
	//					glDeleteTextures(1, &diffuseMap);	// <-----
	//					glDeleteTextures(1, &normalMap);
}

void ModelLoadingDemo::Initalize()
{
	if (initalizeScene)
	{
		glEnable(GL_DEPTH_TEST);

		std::vector<const GLchar*> faces;
		faces.push_back("Resources/skybox/posx.jpg");
		faces.push_back("Resources/skybox/negx.jpg");
		faces.push_back("Resources/skybox/posy.jpg");
		faces.push_back("Resources/skybox/negy.jpg");
		faces.push_back("Resources/skybox/posz.jpg");
		faces.push_back("Resources/skybox/negz.jpg");
		cubeMapTextureID = ResourceManager::LoadCubeMap(faces);
		faces.clear();	// NOTE(Darren): Memory is not clearned here.

		shaderModelLoading = ResourceManager::GetShader("ModelLoading");
		shaderModelLoading->Use();
		glUniform1i(glGetUniformLocation(shaderModelLoading->Program, "skybox"), 0); // Texture unit.
		shaderNormal = ResourceManager::GetShader("ModelNormal");
		shaderSkybox = ResourceManager::GetShader("Skybox");
		shaderSkybox->Use();
		glUniform1i(glGetUniformLocation(shaderSkybox->Program, "skybox"), 1); // Texture unit.

		modelCyborg = ResourceManager::GetModel("Cyborg");

		initalizeScene = false;
	}
}

void ModelLoadingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	//camera.ControllerMovement();

	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
	Matrix4 modelMatrix;

	shaderModelLoading->Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderModelLoading->Program, "viewMatrix"), 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModelLoading->Program, "projectionMatrix"), 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderModelLoading->Program, "modelMatrix"), 1, GL_FALSE, &modelMatrix.data[0]);

	float lightPosData[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	Matrix4 lightMatrix = lightMatrix.translate(lightPosition);
	lightMatrix = lightMatrix.scale(vector3(0.1f, 0.1f, 0.1f));

	float cameraPosData[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(glGetUniformLocation(shaderModelLoading->Program, "cameraPosition"), 1, &cameraPosData[0]);
	glUniform1i(glGetUniformLocation(shaderModelLoading->Program, "normalMapping"), normalMapping);
	glUniform1i(glGetUniformLocation(shaderModelLoading->Program, "inTangentSpace"), inTangentSpace);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
	modelCyborg->Draw(*shaderModelLoading);

	if (showNormals)
	{
		shaderNormal->Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderNormal->Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderNormal->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderNormal->Program, "model"), 1, GL_FALSE, &modelMatrix.data[0]);
		modelCyborg->Draw(*shaderNormal);
	}

	if (showCubemap)
	{
		shaderSkybox->Use();
		Matrix4 view = camera.GetViewMatrix();
		view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
		glUniformMatrix4fv(glGetUniformLocation(shaderSkybox->Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderSkybox->Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		SceneModels::RenderSkybox();
	}
}