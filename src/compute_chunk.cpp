#include "compute_chunk.h"

ComputeChunk::~ComputeChunk()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &voxel_ssbo);
    glDeleteBuffers(1, &indirect_command);
    glDeleteTextures(1, &palette);
}

void ComputeChunk::generate_buffers()
{
    shader = Shader("../shaders/compute_chunk/compute_chunk.vert", "../shaders/compute_chunk/compute_chunk.frag");

    compute = ComputeShader("../shaders/compute_chunk/compute_chunk.comp");

    const ogt_vox_scene* voxScene = load_vox_scene("../res/vox/45mins.vox");
    std::vector<Voxel> voxel_data;

    for (uint32_t inst_index = 0; inst_index < voxScene->num_instances; inst_index++) {

        const ogt_vox_instance* voxInstance = &voxScene->instances[inst_index];
        const ogt_vox_model* voxModel = voxScene->models[voxInstance->model_index];
        ogt_vox_transform instance_transform = voxInstance->transform;

        glm::vec3 instance_offset(instance_transform.m31, instance_transform.m32, instance_transform.m30);

        uint32_t voxel_index = 0;


        // TODO: use these for work group size
        uint32_t size_x = voxModel->size_x;
        uint32_t size_y = voxModel->size_y;
        uint32_t size_z = voxModel->size_z;

        // bind voxel_data of current instance to buffer
        // count vertex count of instance in compute shader to use as base offset for next instance's vertices

        for (uint32_t z = 0; z < size_z; z++)
        {
            for (uint32_t y = 0; y < size_y; y++)
            {
                for (uint32_t x = 0; x < size_x; x++, voxel_index++)
                {
                    uint32_t color_index = voxModel->voxel_data[voxel_index];
                    bool is_voxel_solid = (color_index != 0);

                    if (!is_voxel_solid) {
                        continue;
                    }

                    glm::vec3 voxel_pos(y, z, x);

                    voxel_pos += instance_offset - glm::vec3(glm::floor(size_y / 2.0), glm::floor(size_z / 2.0), glm::floor(size_x / 2.0));

                    uint32_t neighbours = 0;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_x0(x, y - 1.0, z);
                    bool add_x0 = (neigh_x0.y < 0.0);
                    if (!add_x0) {
                        uint32_t x0_index = (neigh_x0.z * size_y * size_x) + (neigh_x0.y * size_x) + neigh_x0.x;
                        add_x0 = (voxModel->voxel_data[x0_index] == 0);
                    }

                    neighbours = add_x0 ? (neighbours << 1) | 1 : neighbours << 1;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_x1(x, y + 1.0, z);
                    bool add_x1 = (neigh_x1.y >= size_y);
                    if (!add_x1) {
                        uint32_t x1_index = (neigh_x1.z * size_y * size_x) + (neigh_x1.y * size_x) + neigh_x1.x;
                        add_x1 = (voxModel->voxel_data[x1_index] == 0);
                    }

                    neighbours = add_x1 ? (neighbours << 1) | 1 : neighbours << 1;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_z0(x - 1.0, y, z);
                    bool add_z0 = (neigh_z0.x < 0.0);
                    if (!add_z0) {
                        uint32_t z0_index = (neigh_z0.z * size_y * size_x) + (neigh_z0.y * size_x) + neigh_z0.x;
                        add_z0 = (voxModel->voxel_data[z0_index] == 0);
                    };

                    neighbours = add_z0 ? (neighbours << 1) | 1 : neighbours << 1;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_z1(x + 1.0, y, z);
                    bool add_z1 = (neigh_z1.x >= size_x);
                    if (!add_z1) {
                        uint32_t z1_index = (neigh_z1.z * size_y * size_x) + (neigh_z1.y * size_x) + neigh_z1.x;
                        add_z1 = (voxModel->voxel_data[z1_index] == 0);
                    }

                    neighbours = add_z1 ? (neighbours << 1) | 1 : neighbours << 1;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_y0(x, y, z - 1.0);
                    bool add_y0 = (neigh_y0.z < 0.0);
                    if (!add_y0) {
                        uint32_t y0_index = (neigh_y0.z * size_y * size_x) + (neigh_y0.y * size_x) + neigh_y0.x;
                        add_y0 = (voxModel->voxel_data[y0_index] == 0);
                    }

                    neighbours = add_y0 ? (neighbours << 1) | 1 : neighbours << 1;

                    //correct order: (y, z, x) : x -> y / y -> z / z -> x
                    glm::vec3 neigh_y1(x, y, z + 1.0);
                    bool add_y1 = (neigh_y1.z >= size_z);
                    if (!add_y1) {
                        uint32_t y1_index = (neigh_y1.z * size_y * size_x) + (neigh_y1.y * size_x) + neigh_y1.x;
                        add_y1 = (voxModel->voxel_data[y1_index] == 0);
                    }

                    neighbours = add_y1 ? (neighbours << 1) | 1 : neighbours << 1;

                    neighbours = neighbours << 2; // last values are 0 to fill full byte

                    uint32_t data = (color_index << 8) | neighbours;

                    voxel_data.push_back(Voxel{voxel_pos, data});
                }
            }
        }
    }

    DrawArraysIndirectCommand indirect_data{};
    indirect_data.count = 0;
    indirect_data.instanceCount = 1;
    indirect_data.first = 0;
    indirect_data.baseInstance = 0;

    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &voxel_ssbo);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &indirect_command);

    glNamedBufferStorage(voxel_ssbo, voxel_data.size() * sizeof(Voxel), &voxel_data[0], GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(vbo, voxel_data.size() * 36 * sizeof(Vertex), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT); 
    // TODO: size adjustable somehow?
    glNamedBufferStorage(indirect_command, sizeof(DrawArraysIndirectCommand), &indirect_data,
      GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    // position attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // color index
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 1, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(GLfloat));

    // normal
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat) + sizeof(uint32_t));

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    //compute shader call
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxel_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indirect_command);

    ogt_vox_palette ogt_palette = voxScene->palette;

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

    compute.use();

    glDispatchCompute(voxel_data.size(), 1, 1);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT | 
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );

    ogt_vox_destroy_scene(voxScene);
}

void ComputeChunk::render(glm::mat4 mvp, float current_frame)
{
    // fragment/ vertex shader calls
    glBindTextureUnit(0, palette);

    shader.use();
    shader.setInt("palette", 0);
    shader.setVec3("light_direction", -0.45f, -0.7f, -0.2f);

    shader.setMat4("mvp", mvp);

    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_command);
    glDrawArraysIndirect(GL_TRIANGLES, 0);

    glBindVertexArray(0);
}