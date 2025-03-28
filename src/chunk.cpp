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

Chunk::Chunk(glm::vec3 chunk_size){
    size = chunk_size;
    vao = 0;
    vbo = 0;
}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Chunk::generate_buffers()
{
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // bind the Vertes Arrays Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // binds VBO to be used for calls on the GL_ARRAY_BUFFER target

    std::vector<Vertex> buffer;

    const ogt_vox_scene* voxScene = load_vox_scene_with_groups("../res/vox/castle.vox");

    for (uint32_t model_index = 0; model_index < voxScene->num_models; model_index++) {
        const ogt_vox_model* voxModel = voxScene->models[model_index];
        std::cout << voxModel << " " << model_index << std::endl;
    }
    const ogt_vox_model* voxModel = voxScene->models[0];
    uint32_t voxel_index = 0;

    for (uint32_t y = 0; y < voxModel->size_y; y++)
    {
        for (uint32_t z = 0; z < voxModel->size_z; z++)
        {
            for (uint32_t x = 0; x < voxModel->size_x; x++, voxel_index++)
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

                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color});
                buffer.push_back(Vertex{ glm::vec3(1.0f,  0.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f,  1.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f,  1.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos,  glm::vec3(0.0f,  0.0f, -1.0f), vertex_color });

                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 0.0f, 1.0f), vertex_color });

                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  1.0f) + voxel_pos, glm::vec3(-1.0f,  0.0f,  0.0f), vertex_color });

                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(1.0f, 0.0f, 0.0f), vertex_color });

                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, -1.0f, 0.0f), vertex_color });
                
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + voxel_pos, glm::vec3(0.0f, 1.0f, 0.0f), vertex_color });
            
                vertex_count += 36;
            }
        }
    }
     
    std::cout << "Buffer Size: " << buffer.size() << std::endl;

    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(Vertex), &buffer[0], GL_STATIC_DRAW); // copies vertex data into the buffer's memory
    // TODO: Maybe change GL_STATIC_DRAW to GL_DYNAMIC_DRAW if a lot of changes in the vertices should occur

    // position attribute in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal attribute in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //// color attribute in the vertex shader
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    delete[] voxScene;
}

void Chunk::render()
{
    // draws the triangle
    glBindVertexArray(vao); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized


    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //glBindVertexArray(0); //no need to unbind it every time
}
