#include "chunk.h"

#define OGT_VOX_IMPLEMENTATION
#include "ogt_vox/ogt_vox.h"
#include "ogt_vox/vox.h"

#include <iostream>

typedef struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Chunk::generate_buffers()
{
    shader = Shader("../shaders/main.vert", "../shaders/main.frag");

    std::vector<Vertex> buffer;

    const ogt_vox_scene* voxScene = load_vox_scene_with_groups("../res/vox/orientation_test.vox");

    for (uint32_t inst_index = 0; inst_index < voxScene->num_instances; inst_index++) {

        const ogt_vox_instance* voxInstance = &voxScene->instances[inst_index];
        const ogt_vox_model* voxModel = voxScene->models[voxInstance->model_index];
        ogt_vox_transform instance_transform = voxInstance->transform;

        glm::vec3 instance_offset(instance_transform.m30, instance_transform.m31, instance_transform.m32);

        //std::cout   << voxInstance->name << " "
        //            << instance_offset.x << " "
        //            << instance_offset.y << " "
        //            << instance_offset.z << std::endl;

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

                    glm::vec3 voxel_pos(x, z, y);

                    voxel_pos -= instance_offset - glm::vec3(size_z / 2.0, size_y / 2.0, size_x / 2.0);

                    ogt_vox_rgba color = voxScene->palette.color[color_index];
                    glm::vec3 vertex_color(float(color.r), float(color.g), float(color.b));
                    vertex_color /= 255.0f;

                    glm::vec3 neigh_z0(x, y, z - 1.0f);
                    //bool add_z0 = (neigh_z0.z < 0.0);
                    bool add_z0 = true;
                    if (!add_z0) {
                        uint32_t z0_index = (neigh_z0.y * size_z * size_x) + (neigh_z0.z * size_x) + neigh_z0.x;
                        add_z0 = (voxModel->voxel_data[z0_index] == 0);
                    }

                    if (add_z0) {

                        glm::vec3 normal(-1.0f, 0.0f, 0.0f);

                        buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  1.0f) + voxel_pos, normal, vertex_color });

                        vertex_count += 6;
                    }

                    glm::vec3 neigh_z1(x, y, z + 1.0f);
                    //bool add_z1 = (neigh_z1.z >= size_z)/
                    bool add_z1 = true;
                    if (!add_z1) {
                        uint32_t z1_index = (neigh_z1.y * size_z * size_x) + (neigh_z1.z * size_x) + neigh_z1.x;
                        add_z1 = (voxModel->voxel_data[z1_index] == 0);
                    }

                    if (add_z1) {

                        glm::vec3 normal(1.0f, 0.0f, 0.0f);

                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });

                        vertex_count += 6;
                    }

                    glm::vec3 neigh_x0(x - 1.0f, y, z);
                    //bool add_x0 = (neigh_x0.x < 0.0);
                    bool add_x0 = true;
                    if (!add_x0) {
                        uint32_t x0_index = (neigh_x0.y * size_z * size_x) + (neigh_x0.z * size_x) + neigh_x0.x;
                        add_x0 = (voxModel->voxel_data[x0_index] == 0);
                    }

                    if (add_x0) {
                        glm::vec3 normal(0.0f, 0.0f, -1.0f);

                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });

                        vertex_count += 6;
                    }

                    glm::vec3 neigh_x1(x + 1.0f, y, z);
                    //bool add_x1 = (neigh_x1.x >= size_x);
                    bool add_x1 = true;
                    if (!add_x1) {
                        uint32_t x1_index = (neigh_x1.y * size_z * size_x) + (neigh_x1.z * size_x) + neigh_x1.x;
                        add_x1 = (voxModel->voxel_data[x1_index] == 0);
                    }

                    if (add_x1) {
                        glm::vec3 normal(0.0f, 0.0f, 1.0f);

                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });

                        vertex_count += 6;
                    }

                    glm::vec3 neigh_y0(x, y - 1.0f, z);
                    //bool add_y0 = (neigh_y0.y < 0.0);
                    bool add_y0 = true;
                    if (!add_y0) {
                        uint32_t y0_index = (neigh_y0.y * size_z * size_x) + (neigh_y0.z * size_x) + neigh_y0.x;
                        add_y0 = (voxModel->voxel_data[y0_index] == 0);
                    }

                    if (add_y0) {
                        glm::vec3 normal(0.0f, -1.0f, 0.0f);

                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });

                        vertex_count += 6;
                    }

                    glm::vec3 neigh_y1(x, y + 1.0f, z);
                    //bool add_y1 = (neigh_y1.y >= size_y);
                    bool add_y1 = true;
                    if (!add_y1) {
                        uint32_t y1_index = (neigh_y1.y * size_z * size_x) + (neigh_y1.z * size_x) + neigh_y1.x;
                        add_y1 = (voxModel->voxel_data[y1_index] == 0);
                    }

                    if (add_y1) {
                        glm::vec3 normal(0.0f, 1.0f, 0.0f);

                        buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                        buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + voxel_pos, normal, vertex_color });

                        vertex_count += 6;
                    }
                }
            }
        }
    }

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, buffer.size() * sizeof(Vertex), &buffer[0], GL_STATIC_DRAW);
    // TODO: Maybe change GL_STATIC_DRAW to GL_DYNAMIC_DRAW if a lot of changes in the vertices should occur
    
    // position attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // normal attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    // color attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex)); // copies vertex data into the buffer's memory

    delete[] voxScene;
}

void Chunk::render(glm::mat4 mvp)
{
    shader.use();

    shader.setMat4("mvp", mvp);

    // lighting properties
    shader.setVec3("light_direction", -0.45f, -0.7f, -0.2f);
    shader.setVec3("color", 1.0f, 1.0f, 1.0f);

    // draws the triangle
    glBindVertexArray(vao); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glBindVertexArray(0); //no need to unbind it every time
}
