#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/compute.h"

#include <vector>

#include "ogt_wrapper.h"

// holds data of a voxel mesh and logic to render it
class ComputeChunk {
public:
	ComputeChunk() {};

	~ComputeChunk();

	ComputeChunk& operator=(ComputeChunk&& other) noexcept {
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
			compute = std::move(other.compute);

			// Leave the other object in a valid state
			other.vbo = 0;
			other.vao = 0;
			other.texture = 0;
		}
		return *this;
	}

	void generate_buffers();
	void render(glm::mat4 mvp, float current_frame);

	glm::vec3 color;
	GLuint vbo, vao, texture;
	Shader shader;

	const unsigned int TEXTURE_WIDTH = 2048, TEXTURE_HEIGHT = 2048;
	ComputeShader compute;
};