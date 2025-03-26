#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader/shader.h"
#include "camera.h";

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	~Renderer() {
		handle = nullptr;
		delete handle;

		// glfw: terminate, clearing all previously allocated GLFW resources
		//---------------------------------------------------
		glfwTerminate();
	}

	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	void init(uint16_t size_x, uint16_t size_y, bool enable_wireframe = false);
	void loop();
	void processInput();

	GLFWwindow* handle;
	glm::vec2 window_size;
	
	Camera camera;
	float lastX, lastY, deltaTime, lastFrame;
	bool firstMouse = true;

private:
	Renderer() {};
};