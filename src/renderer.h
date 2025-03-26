#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	GLFWwindow* handle;
	glm::vec2 window_size;

	void init(uint16_t size_x, uint16_t size_y);
	void loop();

	~Renderer() {
		handle = nullptr;
		delete handle;
	}

private:
	Renderer() {};
};