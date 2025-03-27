#pragma once

#include <glad/glad.h>

class Scene {
public:
	Scene();
	~Scene();

	void generate_mesh();
	void render_mesh();

	unsigned int VBO, VAO;
};