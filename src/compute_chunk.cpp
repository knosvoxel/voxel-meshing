#include "compute_chunk.h"

//#include "ogt_vox/ogt_vox.h"
//#include "ogt_vox/vox.h"

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

    std::vector<float> texture_data(TEXTURE_WIDTH * TEXTURE_HEIGHT * 3);

    for (int y = 0; y < TEXTURE_HEIGHT; y++)
    {
        for (int x = 0; x < TEXTURE_WIDTH; x++)
        {
            int index = (x + y * TEXTURE_WIDTH) * 3;
            texture_data[index + 0] = float(x) / TEXTURE_WIDTH;
            texture_data[index + 1] = float(y) / TEXTURE_HEIGHT;
            texture_data[index + 2] = 0.0;
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
