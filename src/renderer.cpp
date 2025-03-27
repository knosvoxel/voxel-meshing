#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int VBO, VAO;

// called every loop to check whether ESC is pressed. If that's the case the window closes
void Renderer::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// checks whether the window has changed size to adjust the viewport too
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* handle, double xposIn, double yposIn)
{
    Renderer& renderer = Renderer::getInstance();

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (renderer.firstMouse)
    {
        renderer.lastX = xpos;
        renderer.lastY = ypos;
        renderer.firstMouse = false;
    }

    float xoffset = xpos - renderer.lastX;
    float yoffset = renderer.lastY - ypos;

    renderer.lastX = xpos;
    renderer.lastY = ypos;

    renderer.camera.ProcessMouseMovement(xoffset, yoffset);
}

void Renderer::init(uint16_t size_x, uint16_t size_y, bool enable_wireframe) {
    window_size = glm::vec2(size_x, size_y);

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    window = glfwCreateWindow(window_size.x, window_size.y, "Voxel Meshing", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // configure global opengl state
    // -------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // Enable culling
    glEnable(GL_FRONT); // Cull front faces
    glFrontFace(GL_CW);

    camera = Camera(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f);
    lastX = window_size.x / 2.0f;
    lastY = window_size.y / 2.0f;
    deltaTime = 0.0f;
    lastFrame = 0.0f;

   // build and compile our shader program
   // ---------------------------------------
    mainShader = Shader("../shaders/shader.vert", "../shaders/shader.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    //-----------------------------------------------------------------
    
    //float vertices[]{
    //    // positions       // colors
    //    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// bottom right
    //   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom left
    //    0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top right
    //    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top left
    //    0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,// top right
    //    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f// bottom left
    //};

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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

    //// normal atribute in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // uncomment this call to draw in wireframe polygons.
    if (enable_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput();

        // render
        glClearColor(0.149f, 0.149f, 0.149f, 1.0f); // color to use glClear with
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the color buffer "replacing" all pixels with the selected color

        mainShader.use();

        mainShader.setVec3("light_direction", 0.45f, -0.7f, 0.2f);
        mainShader.setVec3("color", 1.0f, 1.0f, 1.0f);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_size.x / (float)window_size.y, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        mainShader.setMat4("projection", projection);
        mainShader.setMat4("view", view);
        mainShader.setMat4("model", model);

        // draws the triangle
        glBindVertexArray(VAO); //not necessary here because we only have a single VAO but doing it anyway to keep things more organized


        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0); //no need to unbind it every time


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}