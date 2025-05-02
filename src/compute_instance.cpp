#include "compute_instance.h"

ComputeInstance::~ComputeInstance()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &voxel_ssbo);
    glDeleteBuffers(1, &indirect_command);
    glDeleteBuffers(1, &instance_data_buffer);
}

void ComputeInstance::generate_buffers(const ogt_vox_model* model, glm::vec4 offset)
{
    uint32_t size_x = model->size_x;
    uint32_t size_y = model->size_y;
    uint32_t size_z = model->size_z;

    const uint8_t* voxel_data = model->voxel_data;

    InstanceData instance_data{};
    instance_data.instance_size = glm::vec4(size_x, size_y, size_z, 0);
    instance_data.instance_position_offset = offset;

    DrawArraysIndirectCommand indirect_data{};
    indirect_data.count = 0;
    indirect_data.instanceCount = 1;
    indirect_data.first = 0;
    indirect_data.baseInstance = 0;

    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &voxel_ssbo);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &indirect_command);
    glCreateBuffers(1, &instance_data_buffer);

    glNamedBufferStorage(voxel_ssbo, sizeof(uint8_t) * size_x * size_y * size_z, voxel_data, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(vbo, sizeof(Vertex) * size_x * size_y * size_z * 36, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glNamedBufferStorage(indirect_command, sizeof(DrawArraysIndirectCommand), &indirect_data,
      GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glNamedBufferStorage(instance_data_buffer, sizeof(InstanceData), &instance_data, GL_DYNAMIC_STORAGE_BIT |GL_MAP_READ_BIT);

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
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, instance_data_buffer);

    glDispatchCompute(size_x, size_y, size_z);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT | 
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );
}

void ComputeInstance::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_command);
    glDrawArraysIndirect(GL_TRIANGLES, 0);

    glBindVertexArray(0);
}