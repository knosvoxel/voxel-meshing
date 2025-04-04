#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "learnopengl/shader.h"

// holds data of a voxel mesh and logic to render it
class Chunk {
public:
	Chunk() {};
	~Chunk();

	void generate_buffers();
	void render(glm::mat4 mvp);

	GLuint vbo, vao, vertex_count = 0;
	Shader shader;
};