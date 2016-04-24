// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "Imgui/imgui.h"
#include "Imgui_impl_glfw.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(1100, 600, "OpenGL Tech Demo - Darren Sweeney", NULL, NULL);
	glfwSetWindowPos(window, 80, 80);
    glfwMakeContextCurrent(window);

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window, true);

	bool windowOpened = true;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		ImGui_ImplGlfw_NewFrame();

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
        ImGui::Render();

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}