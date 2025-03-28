#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/camera.h";
#include "chunk.h";

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	~Renderer() {
		window = nullptr;
		delete window;

		// glfw: terminate, clearing all previously allocated GLFW resources
		//---------------------------------------------------
		glfwTerminate();
	}

	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	void init(uint16_t size_x, uint16_t size_y, bool enable_wireframe = false);
	void loop();
	void processInput();

	GLFWwindow* window;
	glm::vec2 window_size;
	
	Shader mainShader;
	Camera camera;
	Chunk chunk;

	float lastX, lastY, deltaTime, lastFrame;
	bool firstMouse = true;

private:
	Renderer() {};
};