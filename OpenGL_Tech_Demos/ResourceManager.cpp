#include "ResourceManager.h"

// Instantiate static variables
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string,  Model>       ResourceManager::Models;

void ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
}

Shader* ResourceManager::GetShader(std::string name)
{
	return &Shaders[name];
}

void ResourceManager::LoadModel(const string path, bool loadTangent, std::string name)
{
	Models[name] = loadModel(path, loadTangent);
}

Model* ResourceManager::GetModel(std::string name)
{
	return &Models[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.Program);
	// TODO(Darren): Delete all the models.
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath)
{
	// Replace the vertex/fragment source code from filepath
	std::string vertexCode, fragmentCode, geometryCode;
	std::ifstream vShaderFile, fShaderFile, gShaderFile;

	// Ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	if (geometryPath != NULL)
		gShaderFile.exceptions(std::ifstream::badbit);

	try
	{
		// Open file.
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		if (geometryPath != NULL)
			gShaderFile.open(geometryPath);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		// Read file's buffer contents into steam
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		if (geometryPath != NULL)
			gShaderStream << gShaderFile.rdbuf();
		// Cose file handlers
		vShaderFile.close();
		fShaderFile.close();
		if (geometryPath != NULL)
			gShaderFile.close();

		// Convert stream in GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometryPath != NULL)
			geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = (geometryPath != NULL ? geometryCode.c_str() : NULL);

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderCode != NULL ? gShaderCode : NULL);

	return shader;
}

Model ResourceManager::loadModel(const string path, bool loadTangent)
{
	Model model;
	model.LoadModel(path, loadTangent);
	return model;
}

GLuint ResourceManager::LoadTexture(GLchar *path, GLboolean gammaCorrection, GLboolean alpha, GLboolean anisotropicFilter)
{
	// Generate a texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	if (!image)
		std::cout << "ERROR_RESOUCRE_MANAGER::Image was not loaded!" << std::endl;
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	// TODO(Darren): Fix this.
	//glTexImage2D(GL_TEXTURE_2D, 0, gammaCorrection ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

	// Parameters
	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders when alpha is true.
	// Due to interpolation it takes value from next repeat 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

GLuint ResourceManager::LoadCubeMap(std::vector<const GLchar*> faces)
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

// TODO(Darren): Need to start loading textures.
void ResourceManager::LoadAllDemoResources()
{
	// Model Loading Demo.
	LoadShader("Shaders/ModelLoadingDemo/normal.vert", "Shaders/ModelLoadingDemo/normal.frag", "Shaders/ModelLoadingDemo/normal.gs",
		"ModelNormal");
	LoadShader("Shaders/ModelLoadingDemo/modelLoading.vert", "Shaders/ModelLoadingDemo/modelLoading.frag", NULL,
		"ModelLoading");
	LoadModel("Resources/model/cyborg.obj", true,
		"Cyborg");

	// High Dynamic Range Lighting Demo.
	LoadShader("Shaders/HDR_Demo/HDR.vert", "Shaders/HDR_Demo/HDR.frag", NULL,
		"HDR");
	LoadShader("Shaders/HDR_Demo/bloom.vert", "Shaders/HDR_Demo/bloom.frag", NULL,
		"Bloom");
	LoadShader("Shaders/HDR_Demo/blur.vert", "Shaders/HDR_Demo/blur.frag", NULL,
		"Blur");

	// Omnidirectional Shadow Mapping Demo.
	LoadShader("Shaders/OmnidirectionalShadowDemo/point_shadows.vert", "Shaders/OmnidirectionalShadowDemo/point_shadows.frag", NULL,
		"PointShadows");
	LoadShader("Shaders/OmnidirectionalShadowDemo/point_shadows_depth.vert", "Shaders/OmnidirectionalShadowDemo/point_shadows_depth.frag", "Shaders/OmnidirectionalShadowDemo/point_shadows_depth.gs", 
		"PointShadowsDepth");
	LoadModel("Resources/platform.obj", false,
		"Platform");
	LoadModel("Resources/column.obj", false,
		"Column");
	LoadModel("Resources/bunny.obj", false,
		"Bunny");

	// Instancing Demo.
	LoadShader("Shaders/InstancingDemo/instance.vert", "Shaders/InstancingDemo/grass.frag", NULL,
		"Instancing");
	LoadModel("Resources/rock/rock.obj", false,
		"Rock");

	// Stencil Reflection Demo.
	LoadShader("Shaders/StencilReflectionDemo/lighting.vert", "Shaders/StencilReflectionDemo/lighting.frag", NULL,
		"Lighting");
	LoadModel("Resources/utah-teapot.obj", false,
		"Utah_Teapot");
	LoadModel("Resources/tree1b_lod0_2.obj", false,
		"Tree");

	// Object Outline Demo.
	LoadShader("Shaders/ObjectOutlineDemo/StencilTesting.vert", "Shaders/ObjectOutlineDemo/StencilTesting.frag", NULL,
		"StencilTesting");
	LoadShader("Shaders/ObjectOutlineDemo/StencilTesting.vert", "Shaders/ObjectOutlineDemo/ObjectOutline.frag", NULL,
		"ObjectOutline");

	// Deferred Rendering Demo.
	LoadShader("Shaders/DeferredRendering/g_buffer.vert", "Shaders/DeferredRendering/g_buffer.frag", NULL,
		"g_buffer");
	LoadShader("Shaders/DeferredRendering/deferred_shading.vert", "Shaders/DeferredRendering/deferred_shading.frag", NULL,
		"DeferredShading");

	// CubeMap Demo.
	LoadShader("Shaders/CubeMapDemo/model.vert", "Shaders/CubeMapDemo/model.frag", NULL,
		"Model");
	LoadShader("Shaders/CubeMapDemo/skybox.vert", "Shaders/CubeMapDemo/skybox.frag", NULL,
		"Skybox");

	// Directional Shadow Mapping Demo.
	LoadShader("Shaders/ShadowMapDemo/ShadowMapping.vert", "Shaders/ShadowMapDemo/ShadowMapping.frag", NULL,
		"ShadowMapping");
	LoadShader("Shaders/ShadowMapDemo/ShadowMappingDepth.vert", "Shaders/ShadowMapDemo/ShadowMappingDepth.frag", NULL,
		"ShadowMappingDepth");
	LoadShader("Shaders/ShadowMapDemo/debugQuadDepth.vert", "Shaders/ShadowMapDemo/debugQuadDepth.frag", NULL,
		"DebugQuadDepth");

	// Parallax Mapping Demo.
	LoadShader("Shaders/ParallaxingMappingDemo/parallax_mapping.vert", "Shaders/ParallaxingMappingDemo/parallax_mapping.frag", NULL,
		"ParallaxMapping");

	// Screen Space Ambient Occlusion Demo.
	LoadShader("Shaders/SSAO_Demo/ssao_geometry.vert", "Shaders/SSAO_Demo/ssao_geometry.frag", NULL,
		"ssao_geometry");
	LoadShader("Shaders/SSAO_Demo/ssao.vert", "Shaders/SSAO_Demo/ssao_lighting.frag", NULL,
		"ssao_lighting");
	LoadShader("Shaders/SSAO_Demo/ssao.vert", "Shaders/SSAO_Demo/ssao.frag", NULL,
		"ssao");
	LoadShader("Shaders/SSAO_Demo/ssao.vert", "Shaders/SSAO_Demo/ssao_blur.frag", NULL, 
		"ssao_blur");

	// General Shaders
	LoadShader("Shaders/light_box.vert", "Shaders/light_box.frag", NULL,
		"LightBox");
	LoadShader("Shaders/EnviromentObject.vert", "Shaders/EnviromentObject.frag", NULL,
		"EnviromentObject");

}