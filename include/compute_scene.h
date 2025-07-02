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

enum MeshingAlgorithm {
	default,
	greedy_8x8,
	slicing
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
			remap_to_8s_compute = std::move(other.remap_to_8s_compute);
			apply_rotations_compute = std::move(other.apply_rotations_compute);
			buffer_size_compute = std::move(other.buffer_size_compute);
			meshing_compute = std::move(other.meshing_compute);

			// Leave the other object in a valid state
			other.palette = 0;
		}
		return *this;
	}

	void load(const char* path, MeshingAlgorithm algo, size_t iterations_per_instance = 1);
	void render(glm::mat4 mvp, float current_frame);

	ogt_vox_model apply_rotations(const ogt_vox_scene* scene, uint32_t instance_idx, ComputeShader& compute);

	std::vector<ComputeInstance> instances;

	GLuint palette, voxel_count, vertex_count, instance_temp_ssbo, rotated_temp_ssbo, rotation_data_temp_buffer;

	Shader shader;
	ComputeShader remap_to_8s_compute, apply_rotations_compute, buffer_size_compute, meshing_compute;
};