#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include "ogt_wrapper.h"
#include <learnopengl/compute.h>

typedef struct InstanceData {
	glm::vec4 instance_size;
	glm::vec4 remapped_size;
	glm::vec4 instance_position_offset;
	GLuint voxel_count = 0;
};

typedef struct Vertex {
	glm::vec3 pos;
	uint32_t packed_data; // Bytes | 0: 00000000 | 1: 00000000 | 2: normal index |3: color index |
};

typedef struct DrawArraysIndirectCommand {
	GLuint count;
	GLuint instanceCount;
	GLuint first;
	GLuint baseInstance;
};

// holds data of a voxel mesh and logic to render it
class ComputeInstance {
public:
	ComputeInstance() {};

	~ComputeInstance();

	ComputeInstance& operator=(ComputeInstance&& other) noexcept {
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

			// Leave the other object in a valid state
			other.vbo = 0;
			other.vao = 0;
			other.voxel_ssbo = 0;
			other.indirect_command = 0;
			other.instance_data_buffer = 0;
		}
		return *this;
	}

	ComputeInstance(const ComputeInstance&) = default;
	ComputeInstance& operator=(const ComputeInstance&) = default;

	ComputeInstance(ComputeInstance&& other) noexcept {
		*this = std::move(other);
	}

	void prepare_model_data(const ogt_vox_model* model, glm::vec4 offset, ComputeShader& compute);
	void calculate_buffer_size(const ogt_vox_model* model, GLuint& voxel_count, ComputeShader& compute);
	void generate_mesh(GLuint& vertex_count, ComputeShader& compute, bool flat_dispatch, double& dispatch_duration);
	void clear_model_data();

	void render();

	GLuint vbo, vao, voxel_ssbo, remapped_ssbo, indirect_command, instance_data_buffer, vbo_size_buffer;
	uint32_t size_x, size_y, size_z, vbo_size;
};