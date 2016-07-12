#include "ModelLoadingDemo.h"

ModelLoadingDemo::ModelLoadingDemo()
{

}

ModelLoadingDemo::~ModelLoadingDemo()
{
	// TODO(Darren):
	// Dealocation? >>> glBindVertexArray(0);
	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//					glBindBuffer(GL_ARRAY_BUFFER, 0);
	//					glDeleteTextures(1, &diffuseMap);
	//					glDeleteTextures(1, &normalMap);
}

void ModelLoadingDemo::Initalize()
{
	glEnable(GL_DEPTH_TEST);

	sceneModel.LoadModel("Resources/model/cyborg.obj", true);
	lightPosition = vector3(0.0f, 0.0f, -5.0f);

	std::vector<const GLchar*> faces;
	faces.push_back("Resources/skybox/posx.jpg");
	faces.push_back("Resources/skybox/negx.jpg");
	faces.push_back("Resources/skybox/posy.jpg");
	faces.push_back("Resources/skybox/negy.jpg");
	faces.push_back("Resources/skybox/posz.jpg");
	faces.push_back("Resources/skybox/negz.jpg");
	cubeMapTexture = ResourceManager::LoadCubeMap(faces);
	faces.clear();

	ResourceManager::GetShader("ModelLoading").Use();
	GLuint ModelLoadingID = ResourceManager::GetShader("ModelLoading").Program;
	shaderModelMatrix = glGetUniformLocation(ModelLoadingID, "modelMatrix");
	shaderProjectionMatrix = glGetUniformLocation(ModelLoadingID, "projectionMatrix");
	shaderViewMatrix = glGetUniformLocation(ModelLoadingID, "viewMatrix");
	shaderCameraPos = glGetUniformLocation(ModelLoadingID, "cameraPosition");
	shaderNormalMapping = glGetUniformLocation(ModelLoadingID, "normalMapping");
	shaderInTangentSpace = glGetUniformLocation(ModelLoadingID, "inTangentSpace");

	ResourceManager::GetShader("ModelNormal").Use();
	GLuint ModelNormalID = ResourceManager::GetShader("ModelNormal").Program;
	shaderNormalModel = glGetUniformLocation(ModelNormalID, "model");
	shaderNormalProjection = glGetUniformLocation(ModelNormalID, "projection");
	shaderNormalView = glGetUniformLocation(ModelNormalID, "view");
}

void ModelLoadingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	camera.ControllerMovement();

	ResourceManager::GetShader("ModelLoading").Use();
	Matrix4 view;
	view = camera.GetViewMatrix();
	Matrix4 projection;
	projection = projection.perspectiveProjection(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(shaderViewMatrix, 1, GL_FALSE, &view.data[0]);
	glUniformMatrix4fv(shaderProjectionMatrix, 1, GL_FALSE, &projection.data[0]);
	glUniformMatrix4fv(shaderModelMatrix, 1, GL_FALSE, &modelMatrix.data[0]);

	float lightPosData[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	Matrix4 lightMatrix = lightMatrix.translate(lightPosition);
	lightMatrix = lightMatrix.scale(vector3(0.1f, 0.1f, 0.1f));

	float cameraPosData[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(shaderCameraPos, 1, &cameraPosData[0]);
	glUniform1i(shaderNormalMapping, normalMapping);
	glUniform1i(shaderInTangentSpace, inTangentSpace);

	glUniform1i(glGetUniformLocation(ResourceManager::GetShader("ModelLoading").Program, "skybox"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	sceneModel.Draw(ResourceManager::GetShader("ModelLoading"));

	if (showNormals)
	{
		ResourceManager::GetShader("ModelNormal").Use();
		glUniformMatrix4fv(shaderNormalView, 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(shaderNormalProjection, 1, GL_FALSE, &projection.data[0]);
		glUniformMatrix4fv(shaderNormalModel, 1, GL_FALSE, &modelMatrix.data[0]);
		sceneModel.Draw(ResourceManager::GetShader("ModelNormal"));
	}

	if (showCubemap)
	{
		ResourceManager::GetShader("Skybox").Use();
		Matrix4 view = camera.GetViewMatrix();
		view.data[12] = 0; view.data[13] = 0; view.data[14] = 0;	// Take away the translation component.
		glUniformMatrix4fv(glGetUniformLocation(ResourceManager::GetShader("Skybox").Program, "view"), 1, GL_FALSE, &view.data[0]);
		glUniformMatrix4fv(glGetUniformLocation(ResourceManager::GetShader("Skybox").Program, "projection"), 1, GL_FALSE, &projection.data[0]);
		glUniform1i(glGetUniformLocation(ResourceManager::GetShader("Skybox").Program, "skybox"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		SceneModels::RenderSkybox();
	}
}