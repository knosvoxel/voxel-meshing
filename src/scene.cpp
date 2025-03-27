#include "scene.h"

float vertices[] = {
     0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,
     0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,

     0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

     0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
     0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
     0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,

     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

     0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
     0.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,

     0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f
};

Scene::Scene(){}


Scene::~Scene()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Scene::generate_mesh()
{
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // bind the Vertes Arrays Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // binds VBO to be used for calls on the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies vertex data into the buffer's memory
    // TODO: Maybe change GL_STATIC_DRAW to GL_DYNAMIC_DRAW if a lot of changes in the vertices should occur

    // position attribute in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal atribute in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Scene::render_mesh()
{
    // draws the triangle
    glBindVertexArray(VAO); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized


    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glBindVertexArray(0); //no need to unbind it every time
}
