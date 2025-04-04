#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "learnopengl/shader.h"

#include <vector>

// holds data of a voxel mesh and logic to render it
class Line {
public:
	Line() {};
	Line(glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 col);

	~Line();

	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	glm::vec3 color;
	GLuint vbo, vao;
	Shader shader;
};