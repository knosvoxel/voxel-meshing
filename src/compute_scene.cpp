#include "compute_scene.h"

ComputeScene::~ComputeScene(){
	glDeleteTextures(1, &palette);
}

void ComputeScene::load(const char* path)
{
	shader = Shader("../shaders/compute/compute_instance.vert", "../shaders/compute/compute_instance.frag");

	remap_to_8s_compute = ComputeShader("../shaders/compute/remap_to_8s.comp");
	buffer_size_compute = ComputeShader("../shaders/compute/calculate_buffer_size.comp");
	compute = ComputeShader("../shaders/compute/compute_instance_greedy_8x8.comp");

	const ogt_vox_scene* vox_scene = load_vox_scene(path);

	// load instances
	for (size_t i = 0; i < vox_scene->num_instances; i++)
	{
		const ogt_vox_instance* curr_instance = &vox_scene->instances[i]; // currently only with one instance
		const ogt_vox_model* curr_model = vox_scene->models[curr_instance->model_index];

		ogt_vox_transform instance_transform = curr_instance->transform;

		glm::vec4 instance_offset(instance_transform.m30, instance_transform.m31, instance_transform.m32, 0);

		remap_to_8s_compute.use();

		// directly create instance within the vector container w/o a temporary value
		instances.emplace_back();
		instances.back().prepare_model_data(curr_model, instance_offset);

		buffer_size_compute.use();

		instances.back().calculate_buffer_size(curr_model, voxel_count);

		compute.use();
		instances.back().generate_mesh(vertex_count);
	}

	// load palette into texture
	ogt_vox_palette ogt_palette = vox_scene->palette;

	// texture generation with DSA
	glCreateTextures(GL_TEXTURE_2D, 1, &palette);

	glTextureParameteri(palette, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(palette, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(palette, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(palette, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(palette, 1, GL_RGBA8, 256, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTextureSubImage2D(palette, 0, 0, 0, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE, ogt_palette.color);
	glBindImageTexture(0, palette, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

	ogt_vox_destroy_scene(vox_scene);
}

void ComputeScene::render(glm::mat4 mvp, float current_frame) 
{
	// fragment/ vertex shader calls
	glBindTextureUnit(0, palette);

	shader.use();
	shader.setInt("palette", 0);
	shader.setVec3("light_direction", -0.45f, -0.7f, -0.2f);

	shader.setMat4("mvp", mvp);

	for (ComputeInstance& instance : instances)
	{
		instance.render();
	}
}