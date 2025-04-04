#include "line.h"

Line::Line(glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 col)
{
    color = col;

    shader = Shader("../shaders/line.vert", "../shaders/line.frag");

    glm::vec3 vertices[]{
        start_pos,
        end_pos
    };

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute in the vertex shader
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3)); // copies vertex data into the buffer's memory
}

Line::~Line()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

    std::cout << "Happens" << std::endl;
}

void Line::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.use();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);

    shader.setVec3("color", color);

    // draws the triangle
    glBindVertexArray(vao);

    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0); //no need to unbind it every time
}
