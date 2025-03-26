#include "renderer.h"

Shader* shaderMain;

unsigned int VBO, VAO;

// called every loop to check whether ESC is pressed. If that's the case the window closes
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// checks whether the window has changed size to adjust the viewport too
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::init(uint16_t size_x, uint16_t size_y, bool enable_wireframe) {
    window_size = glm::vec2(size_x, size_y);

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    handle = glfwCreateWindow(window_size.x, window_size.y, "Voxel Meshing", nullptr, nullptr);
    if (handle == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(handle);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // configure global opengl state
    // -------------------------------------------
    //glEnable(GL_DEPTH_TEST);

   // build and compile our shader program
   // ---------------------------------------
    Shader newShader("../shaders/shader.vs", "../shaders/shader.fs");
    shaderMain = &newShader;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    //-----------------------------------------------------------------
    float vertices[]{
        // positions       // colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// bottom right
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom left
        0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top right
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top left
        0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f// bottom left
    };

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

    // color atribute in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // uncomment this call to draw in wireframe polygons.
    if (enable_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(handle))
    {
        // input
        processInput(handle);

        // render
        glClearColor(0.149f, 0.149f, 0.149f, 1.0f); // color to use glClear with
        glClear(GL_COLOR_BUFFER_BIT); // clears the color buffer "replacing" all pixels with the selected color

        (*shaderMain).use();

        // draws the triangle
        glBindVertexArray(VAO); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glBindVertexArray(0); //no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //------------------------------------------
        glfwSwapBuffers(handle);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}