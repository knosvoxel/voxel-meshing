#include "compute_instance.h"

ComputeInstance::~ComputeInstance()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &voxel_ssbo);
    glDeleteBuffers(1, &indirect_command);
    glDeleteBuffers(1, &instance_data_buffer);
    glDeleteTextures(1, &palette);
}

void ComputeInstance::generate_buffers(int instance_id)
{
    shader = Shader("../shaders/compute/compute_chunk.vert", "../shaders/compute/compute_chunk.frag");

    compute = ComputeShader("../shaders/compute/compute_chunk_v2.comp");

    const ogt_vox_scene* voxScene = load_vox_scene("../res/vox/32x32x32.vox");
    const ogt_vox_instance* voxInstance = &voxScene->instances[instance_id]; // currently only with one instance
    const ogt_vox_model* voxModel = voxScene->models[voxInstance->model_index];
    ogt_vox_transform instance_transform = voxInstance->transform;

    glm::vec4 instance_offset(instance_transform.m30, instance_transform.m31, instance_transform.m32, 0);

    uint32_t size_x = voxModel->size_x;
    uint32_t size_y = voxModel->size_y;
    uint32_t size_z = voxModel->size_z;

    const uint8_t* voxel_data = voxModel->voxel_data;

    InstanceData instance_data{};
    instance_data.instance_size = glm::vec4(size_x, size_y, size_z, 0);
    instance_data.instance_position_offset = instance_offset;

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

    glDispatchCompute(size_x, size_y, size_z);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT | 
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );

    ogt_vox_destroy_scene(voxScene);
}

void ComputeInstance::render(glm::mat4 mvp, float current_frame)
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