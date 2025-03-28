#include "scene.h"

#include <iostream>

typedef struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
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

    for (int z = 0; z < size.z; z++)
    {
        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
            {
                glm::vec3 chunk_pos(x, y, z);

                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f,  0.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f,  1.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f,  1.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + chunk_pos,  glm::vec3(0.0f,  0.0f, -1.0f) });

                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 0.0f, 1.0f) });

                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  0.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  0.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  1.0f,  1.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f,  0.0f,  1.0f) + chunk_pos, glm::vec3(-1.0f,  0.0f,  0.0f) });

                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(1.0f, 0.0f, 0.0f) });

                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 0.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, -1.0f, 0.0f) });
                
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(1.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 1.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
                buffer.push_back(Vertex{ glm::vec3(0.0f, 1.0f, 0.0f) + chunk_pos, glm::vec3(0.0f, 1.0f, 0.0f) });
            
                vertex_count += 36;
            }
        }
    }
     
    std::cout << buffer.size() << std::endl;

    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(Vertex), &buffer[0], GL_STATIC_DRAW); // copies vertex data into the buffer's memory
    // TODO: Maybe change GL_STATIC_DRAW to GL_DYNAMIC_DRAW if a lot of changes in the vertices should occur

    // position attribute in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal atribute in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chunk::render()
{
    // draws the triangle
    glBindVertexArray(vao); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized


    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //glBindVertexArray(0); //no need to unbind it every time
}
