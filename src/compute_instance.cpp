#include "compute_instance.h"

ComputeInstance::~ComputeInstance()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &remapped_ssbo);
    glDeleteBuffers(1, &indirect_command);
    glDeleteBuffers(1, &instance_data_buffer);
}

// Called first: remaps voxel data to multiples of 8
void ComputeInstance::prepare_model_data(const ogt_vox_model* model, glm::vec4 offset)
{
    uint32_t model_size_x = model->size_x;
    uint32_t model_size_y = model->size_y;
    uint32_t model_size_z = model->size_z;

    size_x = (model_size_x + 7) / 8 * 8;
    size_y = (model_size_y + 7) / 8 * 8;
    size_z = (model_size_z + 7) / 8 * 8;

    const uint8_t* voxel_data = model->voxel_data;

    glCreateBuffers(1, &voxel_ssbo);
    glCreateBuffers(1, &remapped_ssbo);

    glNamedBufferStorage(voxel_ssbo, sizeof(uint8_t) * model_size_x * model_size_y * model_size_z, voxel_data, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(remapped_ssbo, sizeof(uint8_t) * size_x * size_y * size_z, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glClearNamedBufferData(remapped_ssbo, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr); // all values are initially 0. 0 = empty voxel

    InstanceData instance_data{};
    instance_data.instance_size = glm::vec4(model_size_x, model_size_y, model_size_z, 0);
    instance_data.remapped_size = glm::vec4(size_x, size_y, size_z, 0);
    instance_data.instance_position_offset = offset;

    glCreateBuffers(1, &instance_data_buffer);

    glNamedBufferStorage(instance_data_buffer, sizeof(InstanceData), &instance_data, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxel_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, remapped_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instance_data_buffer);

    // remap_to_8s
    glDispatchCompute(model_size_x, model_size_y, model_size_z);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT
    );

    glDeleteBuffers(1, &voxel_ssbo);
}


// Called second: Calculates required VBO size
void ComputeInstance::calculate_buffer_size(const ogt_vox_model* model, GLuint& voxel_count)
{
    glCreateBuffers(1, &vbo_size_buffer);

    glNamedBufferStorage(vbo_size_buffer, sizeof(GLuint), nullptr, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glClearNamedBufferData(vbo_size_buffer, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, remapped_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_size_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instance_data_buffer);

    glDispatchCompute(size_x / 8, size_y / 8, size_z / 8);

    // buffer_size_compute
    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT
    );

    void* vbo_size_ptr = glMapNamedBufferRange(vbo_size_buffer, 0, sizeof(GLuint),
        GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    if (vbo_size_ptr == nullptr) {
        printf("Failed to map vbo_size_buffer.\n");
        return;
    }

    vbo_size = *(GLuint*)vbo_size_ptr;

    glUnmapNamedBuffer(vbo_size_buffer);

    // Read back voxel count
    void* ptr = glMapNamedBuffer(instance_data_buffer, GL_READ_ONLY);
    if (ptr) {
        InstanceData* instance_data = (InstanceData*)ptr;
        voxel_count += instance_data->voxel_count;
        glUnmapNamedBuffer(instance_data_buffer);
    }

    glDeleteBuffers(1, &vbo_size_buffer);
}

// Called third: fills VBO with data
void ComputeInstance::generate_mesh(GLuint& vertex_count) {
    DrawArraysIndirectCommand indirect_data{};
    indirect_data.count = 0;
    indirect_data.instanceCount = 1;
    indirect_data.first = 0;
    indirect_data.baseInstance = 0;

    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &indirect_command);

    glNamedBufferStorage(vbo, sizeof(Vertex) * vbo_size // * 32: Left out but technically required
        , nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(indirect_command, sizeof(DrawArraysIndirectCommand), &indirect_data,
        GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);

    // position attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // packed normal & color index data
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribIFormat(vao, 1, 1, GL_UNSIGNED_INT, 3 * sizeof(GLfloat));

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    //compute shader call
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, remapped_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indirect_command);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, instance_data_buffer);

    // compute
    glDispatchCompute(size_x / 8, size_y / 8, size_z / 8);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );

    // Read back vertex count
    void* ptr = glMapNamedBuffer(indirect_command, GL_READ_ONLY);
    if (ptr) {
        DrawArraysIndirectCommand* command_data = (DrawArraysIndirectCommand*)ptr;
        vertex_count += command_data->count;
        glUnmapNamedBuffer(indirect_command);
    }
};

void ComputeInstance::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_command);
    glDrawArraysIndirect(GL_TRIANGLES, 0);

    glBindVertexArray(0);
}