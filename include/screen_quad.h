#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/compute.h"

#include <vector>

// holds data of a voxel mesh and logic to render it
class ScreenQuad {
public:
	ScreenQuad() {};
	ScreenQuad(float test);

	~ScreenQuad();

	ScreenQuad& operator=(ScreenQuad&& other) noexcept {
		if (this != &other) {
			// Clean up existing resources
			if (vbo) glDeleteBuffers(1, &vbo);
			if (vao) glDeleteVertexArrays(1, &vao);

			// Move data
			color = std::move(other.color);
			vbo = other.vbo;
			vao = other.vao;
			texture = other.texture;
			shader = std::move(other.shader);
			computeShader = std::move(other.computeShader);

			// Leave the other object in a valid state
			other.vbo = 0;
			other.vao = 0;
			other.texture = 0;
		}
		return *this;
	}

	void render(glm::mat4 mvp);

	glm::vec3 color;
	GLuint vbo, vao, texture;
	Shader shader;

	const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;
	ComputeShader computeShader;
};