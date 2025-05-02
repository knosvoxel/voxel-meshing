#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/compute.h"

#include <vector>

#include "compute_instance.h"

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
			compute = std::move(other.compute);

			// Leave the other object in a valid state
			other.palette = 0;
		}
		return *this;
	}

	void load(const char* path);
	void render(glm::mat4 mvp, float current_frame);

	std::vector<ComputeInstance> instances;

	GLuint palette;

	Shader shader;
	ComputeShader compute;
};