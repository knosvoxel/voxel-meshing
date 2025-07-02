#include "compute_scene.h"

static inline glm::mat4 compute_transform_mat(const glm::mat4 transform, const glm::vec3& pivot) {
	static const glm::mat4 shift_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
	const glm::mat4 pivot_matrix = glm::translate(glm::mat4(1.0f), -pivot);
	const glm::mat4 combined_matrix = transform * shift_matrix * pivot_matrix;
	return combined_matrix;
}

inline glm::ivec3 calc_transform(const glm::mat4& mat, const glm::vec3& pos)
{
	return glm::floor(mat * glm::vec4(pos, 1.0f));
}

static inline glm::vec4 instance_pivot(const ogt_vox_model* model) {
	return floor(glm::vec4(model->size_x / 2, model->size_y / 2, model->size_z / 2, 0.0f));
}

static inline glm::vec3 volume_size(const ogt_vox_model* model) {
	return glm::vec3(model->size_x - 1, model->size_y - 1, model->size_z - 1);
}


static glm::mat4 ogt_transform_to_glm(const ogt_vox_scene* scene, const ogt_vox_instance& instance, const ogt_vox_model* model)
{
	ogt_vox_transform t = ogt_vox_sample_instance_transform(&instance, 0, scene);
	const glm::vec4 col0(t.m00, t.m01, t.m02, t.m03);
	const glm::vec4 col1(t.m10, t.m11, t.m12, t.m13);
	const glm::vec4 col2(t.m20, t.m21, t.m22, t.m23);
	const glm::vec4 col3(t.m30, t.m31, t.m32, t.m33);
	const glm::vec3& pivot = instance_pivot(model);
	return compute_transform_mat(glm::mat4(col0, col1, col2, col3), pivot);
}

ComputeScene::~ComputeScene(){
	glDeleteTextures(1, &palette);
}

void ComputeScene::load(const char* path, MeshingAlgorithm algo, size_t iterations_per_instance)
{
	// 2D compute dispatch? -- only required for slicing algorithm
	bool flat_dispatch = false;

	shader = Shader("../shaders/compute/compute_instance.vert", "../shaders/compute/compute_instance.frag");

	apply_rotations_compute = ComputeShader("../shaders/compute/apply_rotations.comp");
	remap_to_8s_compute = ComputeShader("../shaders/compute/remap_to_8s.comp");
	buffer_size_compute = ComputeShader("../shaders/compute/calculate_buffer_size.comp");

	// create compute shader based on selected algorithm
	switch (algo)
	{
	case default:
		meshing_compute = ComputeShader("../shaders/compute/compute_instance.comp");
		break;
	case greedy_8x8:
		meshing_compute = ComputeShader("../shaders/compute/compute_instance_greedy_8x8.comp");
		break;
	case slicing:
		meshing_compute = ComputeShader("../shaders/compute/compute_instance_slicing.comp");
		flat_dispatch = true;
		break;
	default:
		meshing_compute = ComputeShader("../shaders/compute/compute_instance.comp");
		break;
	}

	const ogt_vox_scene* vox_scene = load_vox_scene(path);

	const uint32_t num_instances = vox_scene->num_instances;

	std::cout << iterations_per_instance << " iterations per instance" << std::endl;
	std::cout << vox_scene->num_instances << " instances" << std::endl;;

	double total_duration = 0.0;

	// load instances
	for (size_t i = 0; i < num_instances; i++)
	{
		const ogt_vox_instance* curr_instance = &vox_scene->instances[i]; // currently only with one instance
		const ogt_vox_model* curr_model = vox_scene->models[curr_instance->model_index];

		ogt_vox_transform instance_transform = ogt_vox_sample_instance_transform(curr_instance, 0, vox_scene);

		glm::vec4 instance_offset(instance_transform.m30, instance_transform.m31, instance_transform.m32, 0);

		// directly create instance within the vector container w/o a temporary value
		instances.emplace_back();
		const ogt_vox_model rotated_model = apply_rotations(vox_scene, i, apply_rotations_compute);
		instances.back().prepare_model_data(&rotated_model, instance_offset, remap_to_8s_compute);
		instances.back().calculate_buffer_size(&rotated_model, voxel_count, buffer_size_compute);

		for (size_t i = 0; i < iterations_per_instance; i++)
		{
			// reset vertex count and clear model data at beginning 
			// of each loop to retrieve correct values after loop
			if (iterations_per_instance > 1) {
				vertex_count = 0;
				instances.back().clear_model_data();
			}

			double instance_dispatch_duration = 0.0;
			instances.back().generate_mesh(vertex_count, meshing_compute, flat_dispatch, instance_dispatch_duration);

			total_duration += instance_dispatch_duration;
		}

	}

	std::cout << "Total meshing duration: " << total_duration << "us (" << total_duration / 1000.0 << "ms)" << std::endl;
	std::cout << "Average meshing duration: " << (total_duration / iterations_per_instance) / num_instances << " us" << std::endl;

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

// helper functions and rotation logic based on vengi
// https://github.com/vengi-voxel/vengi/tree/163732702a5685cf591efbe7f2dc5c33fffe8fc7

ogt_vox_model ComputeScene::apply_rotations(const ogt_vox_scene* scene, uint32_t instance_idx, ComputeShader& compute)
{
	const ogt_vox_instance& instance = scene->instances[instance_idx];
	const ogt_vox_model* model = scene->models[instance.model_index];
	glm::mat4& transform_mat = ogt_transform_to_glm(scene, instance, model);

	glm::vec3 corners[8] = {
		{0, 0, 0},
		{model->size_x - 1, 0, 0},
		{0, model->size_y - 1, 0},
		{0, 0, model->size_z - 1},
		{model->size_x - 1, model->size_y - 1, 0},
		{model->size_x - 1, 0, model->size_z - 1},
		{0, model->size_y - 1, model->size_z - 1},
		{model->size_x - 1, model->size_y - 1, model->size_z - 1},
	};

	// transform each corner of bouding box individually
	glm::vec3 min_bounds(FLT_MAX);
	glm::vec3 max_bounds(-FLT_MAX);

	for (int i = 0; i < 8; ++i) {
		glm::vec4 transformed_corner = transform_mat * glm::vec4(corners[i], 1.0f);
		glm::vec3 floored_corner = glm::floor(glm::vec3(transformed_corner));
		min_bounds = glm::min(min_bounds, floored_corner);
		max_bounds = glm::max(max_bounds, floored_corner);
	}

	glm::ivec3 rotated_instance_size = glm::ivec3(max_bounds - min_bounds) + glm::ivec3(1); // +1 since voxel grids are inclusive

	// apply_rotations_compute
	const uint8_t* voxel_data = model->voxel_data;

	glCreateBuffers(1, &instance_temp_ssbo);
	glCreateBuffers(1, &rotated_temp_ssbo);

	glNamedBufferStorage(instance_temp_ssbo, sizeof(uint8_t) * model->size_x * model->size_y * model->size_z, voxel_data, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(rotated_temp_ssbo, sizeof(uint8_t) * model->size_x * model->size_y * model->size_z, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
	glClearNamedBufferData(rotated_temp_ssbo, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr); // all values are initially 0. 0 = empty voxel

	RotationData rotation_data{};
	rotation_data.instance_size = glm::vec4(model->size_x, model->size_y, model->size_z, 1.0);
	rotation_data.rotated_size = glm::vec4(rotated_instance_size, 1.0);
	rotation_data.min_bounds = glm::vec4(min_bounds, 1.0);	
	rotation_data.transform = transform_mat;

	glCreateBuffers(1, &rotation_data_temp_buffer);

	glNamedBufferStorage(rotation_data_temp_buffer, sizeof(RotationData), &rotation_data, GL_DYNAMIC_STORAGE_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_temp_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rotated_temp_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, rotation_data_temp_buffer);

	compute.use();

	// apply_rotations_compute
	glDispatchCompute(model->size_x, model->size_y, model->size_z);

	glMemoryBarrier(
		GL_SHADER_STORAGE_BARRIER_BIT
	);

	ogt_vox_model rotated_model{
		rotated_model.size_x = rotated_instance_size.x,
		rotated_model.size_y = rotated_instance_size.y,
		rotated_model.size_z = rotated_instance_size.z,
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