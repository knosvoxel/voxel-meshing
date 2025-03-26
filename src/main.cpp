#include "renderer.h"

Renderer& renderer = Renderer::getInstance();

int main() {
	
    renderer.init(1920, 1080);

    renderer.loop();

    // glfw: terminate, clearing all previously allocated GLFW resources
    //---------------------------------------------------
    glfwTerminate();

	return 0;
}