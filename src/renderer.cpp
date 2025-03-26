#include "renderer.h"

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

void Renderer::init(uint16_t size_x, uint16_t size_y) {
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
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(handle))
    {
        // input
        processInput(handle);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // color to use glClear with
        glClear(GL_COLOR_BUFFER_BIT); // clears the color buffer "replacing" all pixels with the selected color

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //------------------------------------------
        glfwSwapBuffers(handle);
        glfwPollEvents();
    }
}