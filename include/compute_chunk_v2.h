#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/compute.h"

#include <vector>

#include "ogt_wrapper.h"

typedef struct InstanceData {
	glm::vec3 instance_size;
	glm::vec3 instance_position_offset;
};

typedef struct Vertex {
	glm::vec3 pos;
	uint32_t color_index;
	glm::vec3 normal;
	uint32_t padding; // needed for std430 alignment to 32 bytes
};

typedef struct DrawArraysIndirectCommand {
	GLuint count;
	GLuint instanceCount;
	GLuint first;
	GLuint baseInstance;
};

// holds data of a voxel mesh and logic to render it
class ComputeChunkV2 {
public:
	ComputeChunkV2() {};

	~ComputeChunkV2();

	ComputeChunkV2& operator=(ComputeChunkV2&& other) noexcept {
		if (this != &other) {
			// Clean up existing resources
			if (vbo) glDeleteBuffers(1, &vbo);
			if (vao) glDeleteVertexArrays(1, &vao);

			// Move data
			vbo = other.vbo;
			vao = other.vao;
			voxel_ssbo = other.voxel_ssbo;
			indirect_command = other.indirect_command;
			instance_data_buffer = other.instance_data_buffer;
			palette = other.palette;
			shader = std::move(other.shader);
			compute = std::move(other.compute);

			// Leave the other object in a valid state
			other.vbo = 0;
			other.vao = 0;
			other.voxel_ssbo = 0;
			other.indirect_command = 0;
			other.instance_data_buffer = 0;
			other.palette = 0;
		}
		return *this;
	}

	void generate_buffers();
	void render(glm::mat4 mvp, float current_frame);

	GLuint vbo, vao, voxel_ssbo, indirect_command, instance_data_buffer, palette;
	
	Shader shader;
	ComputeShader compute;
};