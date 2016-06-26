// NOTE(Darren): May seperate these from the main class.
#include "CubeMapDemo.h"
#include "ShadowMapping.h"
#include "HDR_Demo.h"
#include "StencilReflectionsDemo.h"
#include "InstancingDemo.h"
#include "DeferredRenderingDemo.h"
#include "ObjectOutlineDemo.h"
#include "SSAO_Demo.h"
#include "ParallaxMappingDemo.h"
#include "OmnidirectionalShadowDemo.h"
#include "ModelLoadingDemo.h"

#include "camera.h"

#include "Imgui\imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <map>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include "math.h"

#include <SOIL\SOIL.h>

/*
	Note:(Darren): From the GLFW documentation.
			Do not pass the window size to glViewport or other pixel-based OpenGL calls. 
			The window size is in screen coordinates, not pixels. Use the framebuffer size, which is in pixels, for pixel-based calls.
*/

// TODO(Darren): May create ResourceManager to load textures and Primitive class to render shapes.

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void Do_Movement();

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool activeCamera;
bool windowResized;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(vector3(0.0f, 1.5f, 4.0f));

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// Demos
CubeMapDemo cubeMapDemo;
ShadowMapping shadowMappingDemo;
HDR_DEMO hdrDemo;
StencilReflectionDemo stencilReflectionDemo;
InstancingDemo instancingDemo;
DeferredRenderingDemo deferredRenderingDemo;
ObjectOutlineDemo objectOutlineDemo;
SSAO_Demo ssao_Demo;
ParallaxMappingDemo parallaxingDemo;
OmnidirectionalShadowDemo omnidirectionalShadowDemo;
ModelLoadingDemo modelLoadingDemo;

bool fullscreen = false;
const GLFWvidmode* vidMode;

enum Demos
{
	// NOTE(Darren): Should i combine both stencil reflection and stencil outline
	// into one demo called Stencil Buffer Demo?

	cubeMap,					// ------------
	shadowMap,					// *** DONE ***
	hdr,						// *** DONE ***
	stencilReflection,			// ------------
	instancing,					// *** DONE ***
	deferredRendering,			// *** DONE ***
	objectOutline,				// IN PROGRESS.	
	ssao,						// ------------
	parallaxingMappingDemo,		// *** DONE ***
	omnidirectionalShadow,		// *** DONE ***
	modelLoading				// *** DONE ***
};

Demos demos = Demos::deferredRendering;

const char* demoInfo = " ";

// TODO(Darren): Seperate ImGui stuff. Get credit link.
inline void SetupImGuiStyle(bool bStyleDark_, float alpha_)
{
	ImGuiStyle& style = ImGui::GetStyle();

	// light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	if (bStyleDark_)
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f)
			{
				col.w *= alpha_;
			}
		}
	}
	else
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			if (col.w < 1.00f)
			{
				col.x *= alpha_;
				col.y *= alpha_;
				col.z *= alpha_;
				col.w *= alpha_;
			}
		}
	}
}

int main(int, char**)
{
	GLsizei screenWidth = 800, screenHeight = 600;
	//GLsizei screenWidth = 1500, screenHeight = 800;

    // Setup window
    glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		printf("failed to initialize GLFW.\n");
		return -1;
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Get the desktop resolution.
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	vidMode = glfwGetVideoMode(monitor);
	screenWidth = vidMode->width;
	screenHeight = vidMode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Tech Demo - Darren Sweeney",
		fullscreen ? monitor : NULL, NULL);

	// GLFW input callbacks.
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	// Opitions
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowPos(window, 0, 30);

    glfwMakeContextCurrent(window);
	if (gl3wInit()) 
	{
		printf("failed to initialize OpenGL\n");
		return -1;
	}

    // Setup ImGui binding
	// Set the install_callbacks to false as i am setting up the GLFW input callbacks myself above.
    ImGui_ImplGlfwGL3_Init(window, false);
	SetupImGuiStyle(true, 1.0f);

	bool windowOpened = true;
    ImVec4 clear_color = ImColor(114, 144, 154);

	// TODO(Darren): Rename shadowmappingdemo to directional shadow demo.

	// Initializes scenes.
	//cubeMapDemo.InitalizeScene();
	//shadowMappingDemo.InitalizeScene();
	//hdrDemo.InitalizeScene(screenWidth, screenHeight);
	//parallaxingDemo.Initalize(camera.position);
	//stencilReflectionDemo.InitalizeScene();
	//instancingDemo.InitalizeScene();
	deferredRenderingDemo.InitalizeScene(screenWidth, screenHeight);
	//objectOutlineDemo.InitalizeScene();
	//ssao_Demo.InitalizeScene(screenWidth, screenHeight);
	//omnidirectionalShadowDemo.Initalize();
	//modelLoadingDemo.Initalize();

	// ImGui
	float f1 = 0.1f;
	int i1 = 20;
	bool b1 = false;

	bool wireframeMode = false;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

#if 0
		// On input handling, check if F11 is down.
		if (glfwGetKey(window, GLFW_KEY_F11)) 
		{

			// Toggle fullscreen flag.
			fullscreen = !fullscreen;

			// Close the current window.
			glfwDestroyWindow(window);

			// Setup window
			glfwSetErrorCallback(error_callback);
			if (!glfwInit())
			{
				printf("failed to initialize GLFW.\n");
				return -1;
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

			monitor = glfwGetPrimaryMonitor();
			vidMode = glfwGetVideoMode(monitor);
			screenWidth = vidMode->width;
			screenHeight = vidMode->height;

			// Create the new window.
			window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Tech Demo - Darren Sweeney",
				monitor, NULL);

			// GLFW input callbacks.
			glfwSetKeyCallback(window, key_callback);
			glfwSetCursorPosCallback(window, mouse_callback);
			//glfwSetScrollCallback(window, scroll_callback);
			glfwSetMouseButtonCallback(window, mouse_button_callback);
			glfwSetWindowSizeCallback(window, window_size_callback);

			// Opitions
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwMakeContextCurrent(window);
			if (gl3wInit())
			{
				printf("failed to initialize OpenGL\n");
				return -1;
			}
		}
#endif

		camera.deltaTime = deltaTime;

		//glClearColor(0.3f, 0.4f, 0.7f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwPollEvents();
		Do_Movement();

        ImGui_ImplGlfwGL3_NewFrame();

		//int texture_id = shadowMappingDemo.depthMap;

#pragma region ImGui

		/*ImGui::Begin("Shadow Depth Map", &windowOpened, ImVec2(430, 250), 0.5f, ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(screenWidth - 460, 190));
		ImVec2 uv0 = ImVec2(0, 1);
		ImVec2 uv1 = ImVec2(1, 0);
		ImGui::Image((ImTextureID)texture_id, ImVec2(400, 200), uv0, uv1);
		ImGui::End();*/

		ImGui::Begin("OpenGL Tech Demos", &windowOpened, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(5, 5));
		ImGui::SetWindowSize(ImVec2(255, screenHeight - 10));
		ImGui::Text("Application average:\n\t %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Demos", 0, true, true))
		{
			if (ImGui::TreeNode("Cube Mapping"))
			{
				bool clicked = ImGui::Button("Cube Mapping Demo");
				if(clicked)
					demos = Demos::cubeMap;
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Instancing"))
			{
				bool clicked = ImGui::Button("Instancing Demo");
				if (clicked)
				{
					demos = Demos::instancing;
					demoInfo = "Example showing rendering of 6,000 objects, 3,000 grass and 3,000 rock objects.";
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Defered Rendering"))
			{
				bool clicked = ImGui::Button("Defered Rendering Demo");
				if (clicked)
				{
					demos = Demos::deferredRendering;
					demoInfo = "Defered Rendering of 100 Light sources around 100 models..\n";
				}
				ImGui::Checkbox("Move Lights", &deferredRenderingDemo.moveLights);
				ImGui::Checkbox("Toggle Lights", &deferredRenderingDemo.renderLights);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Model Loading"))
			{
				bool clicked = ImGui::Button("Model Loading Demo");
				if (clicked)
				{
					demos = Demos::modelLoading;
					demoInfo = "I used Assimp libiary to load the model\nMore info here.\n";
				}
				ImGui::Checkbox("Display Normals", &modelLoadingDemo.showNormals);
				ImGui::Checkbox("Display Enviroment Map", &modelLoadingDemo.showCubemap);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Stencil Reflections"))
			{
				bool clicked = ImGui::Button("Stencil Reflection Demo");
				if(clicked)
					demos = Demos::stencilReflection;
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Parralxing Mapping"))
			{
				bool clicked = ImGui::Button("Parralxing Mapping Demo");
				if (clicked)
				{
					demos = Demos::parallaxingMappingDemo;
					demoInfo = "Demostates steep parallax mapping along a walkable cobble floor.";
				}
				ImGui::SliderFloat("Depth", &parallaxingDemo.heightScale, 0.005f, 0.10f, "%.3f");
				ImGui::Checkbox("Enable Parallax", &parallaxingDemo.enableParallax);
				ImGui::Button("Pause Light");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("HDR Lighting"))
			{
				bool clicked = ImGui::Button("HDR Demo");
				if (clicked)
				{
					demos = Demos::hdr;
					demoInfo = "";
				}
				ImGui::SliderFloat("Exposure", &hdrDemo.exposure, 0.1f, 5.0f, "%.3f");
				ImGui::Checkbox("Render Lights", &hdrDemo.renderLights);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Shadow Maps"))
			{
				if (ImGui::TreeNode("Directional Shadow Maps"))
				{
					bool clicked = ImGui::Button("Directional Shadow Demo");
					if(clicked)
						demos = Demos::shadowMap;
					ImGui::Checkbox("Move Light Source", &shadowMappingDemo.moveLight);
					ImGui::Checkbox("Render Light", &shadowMappingDemo.renderLight);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Omnidirectional Shadow Maps"))
				{
					bool clicked = ImGui::Button("Omnidirectional Shadow Demo");
					if (clicked)
						demos = Demos::omnidirectionalShadow;
					ImGui::Checkbox("Move Light Source", &omnidirectionalShadowDemo.moveLight);
					ImGui::Checkbox("Render Light", &omnidirectionalShadowDemo.renderLight);
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO"))
			{
				bool clicked = ImGui::Button("SSAO Demo");
				if (clicked)
					demos = Demos::ssao;
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Stencil Outline"))
			{
				bool clicked = ImGui::Button("Stencil Outline Demo");
				if (clicked)
					demos = Demos::objectOutline;
				ImGui::TreePop();
			}
		}
		if (ImGui::CollapsingHeader("Current Demo Info", 0, true, true))
		{
			ImGui::TextWrapped(demoInfo);
		}
		if (ImGui::CollapsingHeader("Application Info", 0, true, true))
		{
			char* version = (char*)glGetString(GL_VERSION);
			char* renderer = (char*)glGetString(GL_RENDERER);

			ImGui::Text(version);
			ImGui::Text(renderer);
			ImGui::Text("Draw Calls: ");
			ImGui::Text("Post Processing Time: ");

			ImGui::Checkbox("FullScreen", &fullscreen);
		}
		if (ImGui::CollapsingHeader("About", 0, true, true))
		{
			ImGui::Text("OpenGL Tech Demo by Darren Sweeney\n\nWebsite: darrensweeney.net\nEmail: darrensweeneydev@gmail.com\nTwitter: @_DarrenSweeney");
		}

		if (ImGui::CollapsingHeader("Debug Controls", 0, true, true))
		{
			if (ImGui::TreeNode("Camera"))
			{
				ImGui::Checkbox("Fly Camera", &camera.flyCamera);

				ImGui::Text("movement_speed");
				ImGui::SliderFloat("1", &camera.movementSpeed, 0.0f, 50.0f, "%.3f");
				ImGui::Text("camera_speed");
				ImGui::SliderFloat("5", &camera.cameraSpeed, 0.0f, 136.0f, "%.3f");

				// Camera Bobing
				ImGui::Text("camera_ampletude");
				ImGui::SliderFloat("2", &camera.ampletude, 0.0f, 1.0f, "%.36f");
				ImGui::Text("camera_frequincy");
				ImGui::SliderFloat("3", &camera.frequincy, 0.0f, 1.0f, "%.36f");
				ImGui::TreePop();
			}

			ImGui::Checkbox("Wireframe Mode", &wireframeMode);
		}
		ImGui::End();
#pragma endregion

		// Rendering
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);

#pragma region DemoUpdateCalls
		// Render the demo scenes.
		switch (demos)
		{
			case Demos::cubeMap:
			{
				cubeMapDemo.UpdateScene(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::shadowMap:
			{
				shadowMappingDemo.UpdateScene(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::hdr:
			{
				hdrDemo.UpdateScene(camera, screenWidth, screenHeight, windowResized);

				break;
			}

			case Demos::stencilReflection:
			{
				stencilReflectionDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::instancing:
			{
				instancingDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::deferredRendering:
			{
				deferredRenderingDemo.Update(camera, screenWidth, screenHeight, windowResized);

				break;
			}


			case Demos::objectOutline:
			{
				objectOutlineDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::ssao:
			{
				ssao_Demo.Update(camera, screenWidth, screenHeight, windowResized);

				break;
			}

			case Demos::parallaxingMappingDemo:
			{
				parallaxingDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::omnidirectionalShadow:
			{
				omnidirectionalShadowDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

			case Demos::modelLoading:
			{
				modelLoadingDemo.Update(camera, screenWidth, screenHeight);

				break;
			}

		default:
			break;
		}
#pragma endregion

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_STENCIL_TEST);
		// Render the UI.
		ImGui::Render();
		glEnable(GL_STENCIL_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);

		windowResized = false;

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	windowResized = true;
}

#pragma region "User Input"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (keys[GLFW_KEY_H] && action == GLFW_PRESS)
	{
		// Do something here.
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void Do_Movement()
{
	//cubeMapDemo.camera.KeyboardMovement(keys, deltaTime);
	camera.KeyboardMovement(keys, deltaTime);
}

bool first_entered_window = true;
void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
	/*
		The sudden jump in the camera is caused by when the cursor first enters
		the window the mouse_callback funtion is called with a xPos and yPos
		equal to the location of the mouse when entered on screen.
		This is usually a position that is quite a distance away from
		the center of the screen resulting in large offsets.
	*/
	if (first_entered_window)
	{
		lastX = xPos;
		lastY = yPos;
		first_entered_window = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	if(activeCamera)
		camera.MouseMovement(xOffset, yOffset);
}
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
	camera.MouseScroll(yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		activeCamera = true;
	else
		activeCamera = false;
}

#pragma endregion