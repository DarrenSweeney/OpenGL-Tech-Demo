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

	shaderModel.InitShader("normal_mapping.vert", "normal_mapping.frag");
	sceneModel.LoadModel("cyborg/cyborg.obj", true);
	lightPosition = vector3(0.0f, 3.0f, 0.9f);

	shaderModel.Use();
	shaderModelMatrix = glGetUniformLocation(shaderModel.Program, "modelMatrix");
	shaderProjectionMatrix = glGetUniformLocation(shaderModel.Program, "projectionMatrix");
	shaderViewMatrix = glGetUniformLocation(shaderModel.Program, "viewMatrix");
	shaderCameraPos = glGetUniformLocation(shaderModel.Program, "cameraPosition");
	shaderNormalMapping = glGetUniformLocation(shaderModel.Program, "normalMapping");
	shaderInTangentSpace = glGetUniformLocation(shaderModel.Program, "inTangentSpace");
}

void ModelLoadingDemo::Update(Camera &camera, GLsizei screenWidth, GLsizei screenHeight)
{
	shaderModel.Use();
	Matrix4 viewMatrix = camera.GetViewMatrix();
	// TODO(Darren): Takes glm out.
	glm::mat4 projMatrix = glm::perspective(camera.zoom, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(shaderViewMatrix, 1, GL_FALSE, &viewMatrix.data[0]);
	glUniformMatrix4fv(shaderProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniformMatrix4fv(shaderModelMatrix, 1, GL_FALSE, &modelMatrix.data[0]);

	float lightPosData[] = { lightPosition.x, lightPosition.y, lightPosition.z };
	glUniform3fv(glGetUniformLocation(shaderModel.Program, "lightPosition"), 1, &lightPosData[0]);
	Matrix4 lightMatrix = lightMatrix.translate(lightPosition);
	lightMatrix = lightMatrix.scale(vector3(0.1f, 0.1f, 0.1f));

	float cameraPosData[] = { camera.position.x, camera.position.y, camera.position.z };
	glUniform3fv(shaderCameraPos, 1, &cameraPosData[0]);
	glUniform1i(shaderNormalMapping, normalMapping);
	glUniform1i(shaderInTangentSpace, inTangentSpace);

	sceneModel.Draw(shaderModel);
}