

// Cube Map Demo
// NOTE(Darren): May seperate these from the main class.
#include "CubeMapDemo.h"

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

GLuint screenWidth = 1100, screenHeight = 600;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool activeCamera;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// Demos
CubeMapDemo cubeMapDemo;

int main(int, char**)
{
	GLuint screenWidth = 1100, screenHeight = 600;

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
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Tech Demo - Darren Sweeney", NULL, NULL);

	// GLFW input callbacks.
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Opitions
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowPos(window, 80, 80);
    glfwMakeContextCurrent(window);
	if (gl3wInit()) 
	{
		printf("failed to initialize OpenGL\n");
		return -1;
	}

    // Setup ImGui binding
	// Set the install_callbacks to false as i am setting up the GLFW input callbacks myself above.
    ImGui_ImplGlfwGL3_Init(window, false);

	bool windowOpened = true;
    ImVec4 clear_color = ImColor(114, 144, 154);

	cubeMapDemo.InitalizeScene();

	// TODO(Darren): Need to cull faces.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        glfwPollEvents();
		Do_Movement();
		cubeMapDemo.camera.ControllerMovement();

        ImGui_ImplGlfwGL3_NewFrame();

#pragma region ImGui
		ImGui::Begin("Darren Sweeney", &windowOpened, 0);
		ImGui::Text("Application average:\n\t %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Demos", 0, true, true))
		{
			if (ImGui::TreeNode("Cube Mapping"))
			{
				ImGui::Button("Cube Mapping Demo");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Instancing"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Defered Rendering"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Model Loading"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Stencil Reflections"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Parralxing Mapping"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("HDR Lighting"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Shadow Maps"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO"))
			{
				// --- 
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Toon Outline"))
			{
				// --- 
				ImGui::TreePop();
			}
		}
		if (ImGui::CollapsingHeader("Info", 0, true, true))
		{
			ImGui::Text("OpenGL Tech Demo by Darren Sweeney\n\nWebsite: darrensweeney.net\nEmail: darrensweeneydev@gmail.com\nTwitter: @_DarrenSweeney");
		}
		ImGui::End();
#pragma endregion		

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render the demo scenes.
		cubeMapDemo.UpdateScene();

		// Render the UI.
		ImGui::Render();

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}

#pragma region "User Input"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (keys[GLFW_MOUSE_BUTTON_LEFT] && action == GLFW_PRESS)
		std::cout << "Darren Sweeney" << std::endl;

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
	cubeMapDemo.camera.KeyboardMovement(keys, deltaTime);
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
		cubeMapDemo.camera.MouseMovement(xOffset, yOffset);
}
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
	cubeMapDemo.camera.MouseScroll(yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		activeCamera = true;
	else
		activeCamera = false;
}

#pragma endregion