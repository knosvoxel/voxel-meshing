#include "renderer.h"

static Renderer& renderer = Renderer::getInstance();

// Camera settings
const float NEAR = 0.1f;
const float FAR = 10000.0f;

glm::vec3 cam_pos(60.0f, 35.0f, 60.0f);
float yaw = 225.0f;
float pitch = -20.0f;

bool mouse_caught = true;

// called every loop to check whether ESC is pressed. If that's the case the window closes
void Renderer::processInput()
{
    if (!mouse_caught) return;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// callback functions
// -------------------------------------------
// checks whether the window has changed size to adjust the viewport too
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    renderer.window_size = glm::vec2(width, height);
}

// mouse movement
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!mouse_caught) return;

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

// mouse button input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && !mouse_caught)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouse_caught = true;
        renderer.firstMouse = true;
    }
}

// keyboard input
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        if (mouse_caught) 
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouse_caught = false;
        }
        else 
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouse_caught = true;
            renderer.firstMouse = true;
        }
    }
}

void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

// Renderer functions
// -------------------------------------------
void Renderer::imgui_render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Model Data", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Frametime: %.3f ms (FPS %.1f)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Total Draw Call Duration: %.1f us", compute_scene.total_draw_call_duration);
    ImGui::Text("Average Draw Call Duration: %.1f us", compute_scene.total_draw_call_duration / compute_scene.num_instances);
    ImGui::Separator();
    if (ImGui::Checkbox("VSync", &vsync_enabled))
    {
        if (vsync_enabled){
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Wireframe", &wireframe_enabled))
    {
        if (wireframe_enabled) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    ImGui::Separator();
    ImGui::Text("Voxel Count: %1u", compute_scene.voxel_count);
    ImGui::Text("Vertex Count: %1u (%1u faces)", compute_scene.vertex_count, compute_scene.vertex_count / 6);
    ImGui::Separator();
    ImGui::DragFloat3("Position", (float*)&camera.Position, 0.01f);
    ImGui::DragFloat("Movement Speed", (float*)&camera.MovementSpeed, 0.01f, 0.0f, 0.0f, "%.1f"); 
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::configure_algorithm(const char* file_path_to_load, MeshingAlgorithm algorithm, uint16_t meshing_iteration_amount)
{
    vox_file_path = file_path_to_load;
    this->algorithm = algorithm;
    meshing_iterations = meshing_iteration_amount;
}

void Renderer::init(uint16_t size_x, uint16_t size_y, bool enable_vsync, bool enable_wireframe) {
    window_size = glm::vec2(size_x, size_y);
    vsync_enabled = enable_vsync;
    wireframe_enabled = enable_wireframe;

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!enable_vsync) {
        glfwSwapInterval(0);
    }

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

    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(message_callback, 0);

    // overdraw debug visuals
    // also adjust the shader in compute_scene.cpp to use overdraw.frag for this to work correctly
    //glEnable(GL_BLEND);
    //glDepthFunc(GL_ALWAYS);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup Dear ImGui context
    // -------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "../imgui_config.ini";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    // camera setup
    // -------------------------------------------
    camera = Camera(cam_pos, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);
    lastX = window_size.x / 2.0f;
    lastY = window_size.y / 2.0f;
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    // Coordinate lines
    // ---------------------------------------
    coord_x = Line(glm::vec3(0.0, 0.0, 0.0), glm::vec3(128.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
    coord_y = Line(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 128.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    coord_z = Line(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 128.0), glm::vec3(0.0, 0.0, 1.0));
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    //-----------------------------------------------------------------
    compute_scene.load(vox_file_path, algorithm, meshing_iterations);
    //chunk.generate_buffers();
    //quad.generate_buffers();

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
        glClearColor(0.20f, 0.20f, 0.20f, 1.0f); // color to use glClear with

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the color buffer "replacing" all pixels with the selected color

        // camera properties
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_size.x / (float)window_size.y, NEAR, FAR);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 mvp = projection * view * model;

        // render coordinate lines
        coord_x.render(mvp);
        coord_y.render(mvp);
        coord_z.render(mvp);

        // render main object
        compute_scene.render(mvp, currentFrame);
        //chunk.render(mvp);

        // render screen quad
        //float aspect_ratio = window_size.y / window_size.x;

        //glm::vec2 quad_size = glm::vec2(0.25f);
        //glm::vec2 quad_scale = glm::vec2(quad_size.x, quad_size.y / aspect_ratio);

        //glm::vec2 half_size = quad_scale * 0.5f;
        //glm::vec2 quad_pos = glm::vec2(1.0f - half_size.x, 1.0f - half_size.y);

        //glm::mat4 quad_mat = glm::mat4(1.0f);
        //quad_mat = glm::translate(quad_mat, glm::vec3(quad_pos, 0.0f));
        //quad_mat = glm::scale(quad_mat, glm::vec3(quad_scale, 1.0f));


        //quad.render(quad_mat, currentFrame);

        // render imgui UI
        imgui_render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}