#include "screen_quad.h"

ScreenQuad::~ScreenQuad()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void ScreenQuad::generate_buffers() 
{
    shader = Shader("../shaders/screenQuad.vert", "../shaders/screenQuad.frag");

    compute = ComputeShader("../shaders/compute.comp");

    float vertices[]{
        -0.5, -0.5, 0.0, 0.0, 0.0, // vec3 pos vec2 uv
        -0.5, 0.5, 0.0, 0.0, 1.0,
        0.5, -0.5, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.0, 1.0, 1.0,
        0.5, -0.5, 0.0, 1.0, 0.0,
        -0.5, 0.5, 0.0, 0.0, 1.0
    };

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
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}

void ScreenQuad::render(glm::mat4 mvp, float current_frame)
{
    // compute shader call
    compute.use();
    compute.setFloat("t", current_frame);
    
    glDispatchCompute((unsigned int)TEXTURE_WIDTH / 10, (unsigned int)TEXTURE_HEIGHT / 10, 1);

    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // fragment/ vertex shader calls
    glBindTextureUnit(0, texture);

    shader.use();
    shader.setInt("tex", 0);
    shader.setMat4("mvp", mvp);

    // draws the triangle
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0); //no need to unbind it every time


}
