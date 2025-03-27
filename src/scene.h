#pragma once

#include <glad/glad.h>

class Scene {
public:
	Scene();
	~Scene();

	void generate_buffers();
	void render();

	unsigned int VBO, VAO;
};