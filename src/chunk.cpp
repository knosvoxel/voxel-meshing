#include "chunk.h"

#define OGT_VOX_IMPLEMENTATION
#include "ogt_vox.h"
#include "vox.h"

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
    std::vector<Vertex> buffer;

    const ogt_vox_scene* voxScene = load_vox_scene_with_groups("../res/vox/32x32x32.vox");
    const ogt_vox_model* voxModel = voxScene->models[0];

    uint32_t voxel_index = 0;

    uint32_t size_x = voxModel->size_x;
    uint32_t size_y = voxModel->size_y;
    uint32_t size_z = voxModel->size_z;

    for (uint32_t y = 0; y < size_y; y++)
    {
        for (uint32_t z = 0; z < size_z; z++)
        {
            for (uint32_t x = 0; x < size_x; x++, voxel_index++)
            {
                uint32_t color_index = voxModel->voxel_data[voxel_index];
                ogt_vox_rgba color = voxScene->palette.color[color_index];
                bool is_voxel_solid = (color_index != 0);

                if (!is_voxel_solid) {
                    continue;
                }

                glm::vec3 voxel_pos(x, y, z);
                glm::vec3 vertex_color(float(color.r), float(color.g), float(color.b));
                vertex_color /= 255.0f;

                glm::vec3 neigh_minus_z(x, y, z - 1.0);
                bool add_minus_z = (neigh_minus_z.z < 0.0);
                if (!add_minus_z) {
                    uint32_t minus_z_index = (neigh_minus_z.y * size_z * size_x) + (neigh_minus_z.z * size_x) + neigh_minus_z.x;
                    add_minus_z = (voxModel->voxel_data[minus_z_index] == 0);
                }

                if (add_minus_z) {
                    glm::vec3 normal(0.0f, 0.0f, -1.0f);

                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(0.0f, 0.0f, 0.0f), normal, vertex_color });
                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(1.0f, 0.0f, 0.0f), normal, vertex_color });
                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(1.0f, 1.0f, 0.0f), normal, vertex_color });
                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(1.0f, 1.0f, 0.0f), normal, vertex_color });
                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(0.0f, 1.0f, 0.0f), normal, vertex_color });
                    buffer.push_back(Vertex{ voxel_pos + glm::vec3(0.0f, 0.0f, 0.0f), normal, vertex_color });

                    vertex_count += 6;
                }

                glm::vec3 neigh_plus_z(x, y, z + 1.0);
                bool add_plus_z = (neigh_plus_z.z >= size_z);
                if (!add_plus_z) {
                    uint32_t plus_z_index = (neigh_plus_z.y * size_z * size_x) + (neigh_plus_z.z * size_x) + neigh_plus_z.x;
                    add_plus_z = (voxModel->voxel_data[plus_z_index] == 0);
                }

                if (add_plus_z) {
                    glm::vec3 normal(0.0f, 0.0f, 1.0f);

                    buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });

                    vertex_count += 6;
                }

                glm::vec3 neigh_minus_x(x - 1.0, y, z);
                bool add_minus_x = (neigh_minus_x.x < 0.0);
                if (!add_minus_x) {
                    uint32_t minus_x_index = (neigh_minus_x.y * size_z * size_x) + (neigh_minus_x.z * size_x) + neigh_minus_x.x;
                    add_minus_x = (voxModel->voxel_data[minus_x_index] == 0);
                }

                if (add_minus_x) {
                    glm::vec3 normal(-1.0f, 0.0f, 0.0f);

                    buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  1.0f) + voxel_pos, normal, vertex_color });

                    vertex_count += 6;
                }

                glm::vec3 neigh_plus_x(x + 1.0, y, z);
                bool add_plus_x = (neigh_plus_x.x >= size_x);
                if (!add_plus_x) {
                    uint32_t plus_x_index = (neigh_plus_x.y * size_z * size_x) + (neigh_plus_x.z * size_x) + neigh_plus_x.x;
                    add_plus_x = (voxModel->voxel_data[plus_x_index] == 0);
                }

                if (add_plus_x) {
                    glm::vec3 normal(1.0f, 0.0f, 0.0f);

                    buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });

                    vertex_count += 6;
                }

                glm::vec3 neigh_minus_y(x, y - 1.0, z);
                bool add_minus_y = (neigh_minus_y.y < 0.0);
                if (!add_minus_y) {
                    uint32_t minus_y_index = (neigh_minus_y.y * size_z * size_x) + (neigh_minus_y.z * size_x) + neigh_minus_y.x;
                    add_minus_y = (voxModel->voxel_data[minus_y_index] == 0);
                }

                if (add_minus_y) {
                    glm::vec3 normal(0.0f, -1.0f, 0.0f);

                    buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, normal, vertex_color });
                    buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, normal, vertex_color });

                    vertex_count += 6;
                }

                glm::vec3 neigh_plus_y(x, y + 1.0, z);
                bool add_plus_y = (neigh_plus_y.x >= size_y);
                if (!add_plus_y) {
                    uint32_t plus_y_index = (neigh_plus_y.y * size_z * size_x) + (neigh_plus_y.z * size_x) + neigh_plus_y.x;
                    add_plus_y = (voxModel->voxel_data[plus_y_index] == 0);
                }

                if (add_plus_y) {
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

void Chunk::render()
{
    // draws the triangle
    glBindVertexArray(vao); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized


    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //glBindVertexArray(0); //no need to unbind it every time
}
