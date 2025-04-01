#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

// holds data of a voxel mesh
class Chunk {
public:
	Chunk() {};
	//Chunk(glm::vec3 scene_size);
	~Chunk();

	void generate_buffers();
	void render();

	GLuint vbo, vao, vertex_count = 0;
	//glm::vec3 size;
};