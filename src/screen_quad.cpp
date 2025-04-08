#include "screen_quad.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

ScreenQuad::ScreenQuad(float test)
{
    shader = Shader("../shaders/screenQuad.vert", "../shaders/screenQuad.frag");

    computeShader = ComputeShader("../shaders/compute.comp");

    float vertices[]{
        -50.0, -50.0, 0.0, 0.0, 0.0, // vec3 pos vec2 uv
        -50.0, 50.0, 0.0, 0.0, 1.0,
        50.0, -50.0, 0.0, 1.0, 0.0,
        50.0, 50.0, 0.0, 1.0, 1.0,
        50.0, -50.0, 0.0, 1.0, 0.0,
        -50.0, 50.0, 0.0, 0.0, 1.0

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

    //glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    //glTexParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //glTextureStorage2D(texture, 1, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    //glBindTextureUnit(GL_TEXTURE0, texture);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
        GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    /*glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels; 

    unsigned char* data = stbi_load("../res/container.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    stbi_image_free(data);*/
}

ScreenQuad::~ScreenQuad()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void ScreenQuad::render(glm::mat4 mvp)
{
    computeShader.use();
    
    glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT, 1);

    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    shader.setMat4("mvp", mvp);
    shader.setInt("tex", 0);

    // draws the triangle
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0); //no need to unbind it every time


}
