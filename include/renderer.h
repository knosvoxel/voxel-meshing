#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/camera.h";
#include "chunk.h";
#include "line.h";
#include "screen_quad.h"
#include "compute_scene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	~Renderer() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// glfw: terminate, clearing all previously allocated GLFW resources
		//---------------------------------------------------
		glfwTerminate();
	}

	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	void configure_algorithm(const char* file_path_to_load, MeshingAlgorithm algorithm, uint16_t meshing_iteration_amount);
	void init(uint16_t size_x, uint16_t size_y, bool enable_vsync = true, bool enable_wireframe = false);
	void loop();
	void processInput();

	void imgui_render();

	GLFWwindow* window;
	glm::vec2 window_size;
	
	Camera camera;
	ComputeScene compute_scene;
	//Chunk chunk;

	Line coord_x, coord_y, coord_z;
	//ScreenQuad quad;

	const char* vox_file_path = "";
	MeshingAlgorithm algorithm = greedy_8x8;
	uint16_t meshing_iterations = 0;

	float lastX, lastY, deltaTime, lastFrame;
	bool vsync_enabled, wireframe_enabled, firstMouse = true;

private:
	Renderer() {};
};