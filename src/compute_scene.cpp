#include "compute_scene.h"

glm::mat4 ogt_transform_to_glm(const ogt_vox_transform& t)
{
	return glm::mat4(
		t.m00, t.m10, t.m20, t.m30,
		t.m01, t.m11, t.m21, t.m31,
		t.m02, t.m12, t.m22, t.m32,
		t.m03, t.m13, t.m23, t.m33
	);
}

ogt_vox_transform glm_to_ogt_transform(const glm::mat4& m)
{
	ogt_vox_transform t;

	t.m00 = m[0][0]; t.m10 = m[0][1]; t.m20 = m[0][2]; t.m30 = m[0][3];
	t.m01 = m[1][0]; t.m11 = m[1][1]; t.m21 = m[1][2]; t.m31 = m[1][3];
	t.m02 = m[2][0]; t.m12 = m[2][1]; t.m22 = m[2][2]; t.m32 = m[2][3];
	t.m03 = m[3][0]; t.m13 = m[3][1]; t.m23 = m[3][2]; t.m33 = m[3][3];

	return t;
}

ComputeScene::~ComputeScene(){
	glDeleteTextures(1, &palette);
}

void ComputeScene::load(const char* path)
{
	shader = Shader("../shaders/compute/compute_instance.vert", "../shaders/compute/compute_instance.frag");

	apply_rotations_compute = ComputeShader("../shaders/compute/apply_rotations.comp");
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
		
		// directly create instance within the vector container w/o a temporary value
		instances.emplace_back();

		apply_rotations_compute.use();
		
		glm::vec3 instance_size;
		const ogt_vox_model rotated_model = apply_rotations(curr_instance, vox_scene->models, vox_scene->groups, instance_size);

		remap_to_8s_compute.use();
		instances.back().prepare_model_data(&rotated_model, instance_offset);

		buffer_size_compute.use();

		instances.back().calculate_buffer_size(&rotated_model, voxel_count);

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

ogt_vox_model ComputeScene::apply_rotations(const ogt_vox_instance* instance, const ogt_vox_model** models, const ogt_vox_group* groups, glm::vec3& instance_size)
{
	const ogt_vox_model* curr_model = models[instance->model_index];

	glm::mat4 combined_transform = ogt_transform_to_glm(instance->transform);

	uint32_t group_index = instance->group_index;

	while (group_index != k_invalid_group_index) {
		const ogt_vox_group& curr_group = groups[group_index];

		glm::mat4 group_transform = ogt_transform_to_glm(curr_group.transform);
		combined_transform = combined_transform * group_transform;

		group_index = curr_group.parent_group_index;
	}

	glm::vec3 corners[8] = {
		{0, 0, 0},
		{curr_model->size_x, 0, 0},
		{0, curr_model->size_y, 0},
		{0, 0, curr_model->size_z},
		{curr_model->size_x, curr_model->size_y, 0},
		{curr_model->size_x, 0, curr_model->size_z},
		{0, curr_model->size_y, curr_model->size_z},
		{curr_model->size_x, curr_model->size_y, curr_model->size_z},
	};

	// transform each corner of bouding box individually
	glm::vec3 min_bounds(FLT_MAX);
	glm::vec3 max_bounds(-FLT_MAX);

	for (int i = 0; i < 8; ++i) {
		glm::vec4 transformed_corner = combined_transform * glm::vec4(corners[i], 1.0f);
		min_bounds = glm::min(min_bounds, glm::vec3(transformed_corner));
		max_bounds = glm::max(max_bounds, glm::vec3(transformed_corner));
	}

	instance_size = max_bounds - min_bounds;

	// apply_rotations_compute
	const uint8_t* voxel_data = curr_model->voxel_data;

	glCreateBuffers(1, &instance_temp_ssbo);
	glCreateBuffers(1, &rotated_temp_ssbo);

	glNamedBufferStorage(instance_temp_ssbo, sizeof(uint8_t) * curr_model->size_x * curr_model->size_y * curr_model->size_z, voxel_data, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(rotated_temp_ssbo, sizeof(uint8_t) * curr_model->size_x * curr_model->size_y * curr_model->size_z, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
	glClearNamedBufferData(rotated_temp_ssbo, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr); // all values are initially 0. 0 = empty voxel

	RotationData rotation_data{};
	rotation_data.instance_size = glm::vec4(curr_model->size_x, curr_model->size_y, curr_model->size_z, 1.0);
	rotation_data.rotated_size = glm::vec4(instance_size, 1.0);
	rotation_data.min_bounds = glm::vec4(min_bounds, 1.0);
	rotation_data.transform = combined_transform;

	glCreateBuffers(1, &rotation_data_temp_buffer);

	glNamedBufferStorage(rotation_data_temp_buffer, sizeof(RotationData), &rotation_data, GL_DYNAMIC_STORAGE_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_temp_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rotated_temp_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, rotation_data_temp_buffer);

	// remap_to_8s
	glDispatchCompute(curr_model->size_x, curr_model->size_y, curr_model->size_z);

	glMemoryBarrier(
		GL_SHADER_STORAGE_BARRIER_BIT
	);

	ogt_vox_model rotated_model{
		rotated_model.size_x = instance_size.x,
		rotated_model.size_y = instance_size.y,
		rotated_model.size_z = instance_size.z,
		rotated_model.voxel_hash = 0
	};

	// read back model data
	void* ptr = glMapNamedBuffer(rotated_temp_ssbo, GL_READ_ONLY);
	if (ptr) {
		const uint8_t* model_data = (const uint8_t*)ptr;
		rotated_model.voxel_data = model_data;
		glUnmapNamedBuffer(rotated_temp_ssbo);
	}
	else {
		uint8_t null = 0;
		const uint8_t* null_ptr = &null;
		rotated_model.voxel_data = null_ptr;
	}

	glDeleteBuffers(1, &instance_temp_ssbo);
	glDeleteBuffers(1, &rotated_temp_ssbo);
	glDeleteBuffers(1, &rotation_data_temp_buffer);

	return rotated_model;
}