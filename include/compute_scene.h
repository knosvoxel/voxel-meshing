#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/compute.h"

#include <vector>

#include "compute_instance.h"

typedef struct RotationData {
	glm::vec4 instance_size;
	glm::vec4 rotated_size;
	glm::vec4 min_bounds;
	glm::mat4 transform;
};

class ComputeScene {
public:
	ComputeScene() {};

	~ComputeScene();

	ComputeScene& operator=(ComputeScene&& other) noexcept {
		if (this != &other) {
			// Move data
			instances = std::move(other.instances);
			palette = other.palette;
			shader = std::move(other.shader);
			meshing_compute = std::move(other.meshing_compute);

			// Leave the other object in a valid state
			other.palette = 0;
		}
		return *this;
	}

	void load(const char* path);
	void render(glm::mat4 mvp, float current_frame);

	ogt_vox_model apply_rotations(const ogt_vox_scene* scene, uint32_t instance_idx, ComputeShader& compute);

	std::vector<ComputeInstance> instances;

	GLuint palette, voxel_count, vertex_count, instance_temp_ssbo, rotated_temp_ssbo, rotation_data_temp_buffer;

	Shader shader;
	ComputeShader remap_to_8s_compute, apply_rotations_compute, buffer_size_compute, meshing_compute, greedy_8x8_compute;
};