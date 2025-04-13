#include "compute_chunk.h"

ComputeChunk::~ComputeChunk()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void ComputeChunk::generate_buffers()
{
    shader = Shader("../shaders/compute_chunk.vert", "../shaders/compute_chunk.frag");

    compute = ComputeShader("../shaders/compute_chunk.comp");

    float vertices[]{
        -50.0, -50.0, 0.0, 0.0, 0.0, // vec3 pos vec2 uv
        -50.0, 50.0, 0.0, 0.0, 1.0,
        50.0, -50.0, 0.0, 1.0, 0.0,
        50.0, 50.0, 0.0, 1.0, 1.0,
        50.0, -50.0, 0.0, 1.0, 0.0,
        -50.0, 50.0, 0.0, 0.0, 1.0
    };

    const ogt_vox_scene* voxScene = load_vox_scene("../res/vox/32x32x32.vox");

    std::vector<float> texture_data(TEXTURE_WIDTH * TEXTURE_HEIGHT * 3);

    for (uint32_t inst_index = 0; inst_index < voxScene->num_instances; inst_index++) {

        const ogt_vox_instance* voxInstance = &voxScene->instances[inst_index];
        const ogt_vox_model* voxModel = voxScene->models[voxInstance->model_index];
        ogt_vox_transform instance_transform = voxInstance->transform;

        glm::vec3 instance_offset(instance_transform.m31, instance_transform.m32, instance_transform.m30);

        uint32_t voxel_index = 0;

        uint32_t size_x = voxModel->size_x;
        uint32_t size_y = voxModel->size_y;
        uint32_t size_z = voxModel->size_z;

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

                    voxel_pos += instance_offset - glm::vec3(size_y / 2.0, size_z / 2.0, size_x / 2.0);

                    if (voxel_pos.y > 0.0) continue;

                    ogt_vox_rgba color = voxScene->palette.color[color_index];
                    glm::vec3 vertex_color(float(color.r), float(color.g), float(color.b));
                    vertex_color /= 255.0f;

                    int index = (x + y * TEXTURE_WIDTH) * 3;
                    texture_data[index + 0] = vertex_color.r;
                    texture_data[index + 1] = vertex_color.g;
                    texture_data[index + 2] = vertex_color.b;
                }
            }
        }
    }

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // uv attribute
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float)); // copies vertex data into the buffer's memory

    // texture generation with DSA
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(texture, 1, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(texture, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGB, GL_FLOAT, texture_data.data());
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    delete[] voxScene;
}

void ComputeChunk::render(glm::mat4 mvp, float current_frame)
{
    //// compute shader call
    //compute.use();

    //glDispatchCompute((unsigned int)TEXTURE_WIDTH / 10, (unsigned int)TEXTURE_HEIGHT / 10, 1);

    //// make sure writing to image has finished before read
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // fragment/ vertex shader calls
    glBindTextureUnit(0, texture);

    shader.use();
    shader.setInt("tex", 0);

    mvp = glm::translate(mvp, glm::vec3(75.0f, 15.0f, 0.0f));
    mvp = glm::rotate(mvp, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    shader.setMat4("mvp", mvp);

    // draws the triangle
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0); //no need to unbind it every time


}
