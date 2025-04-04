#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/camera.h";
#include "chunk.h";
#include "line.h";

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

	void init(uint16_t size_x, uint16_t size_y, bool enable_vsync = true, bool enable_wireframe = false);
	void loop();
	void processInput();

	GLFWwindow* window;
	glm::vec2 window_size;
	
	Shader mainShader;
	Camera camera;
	Chunk chunk;

	Line coord_x, coord_y, coord_z;

	float lastX, lastY, deltaTime, lastFrame;
	bool firstMouse = true;

private:
	Renderer() {};
};