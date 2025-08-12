#include "renderer.h"

uint16_t window_size_x = 1920;
uint16_t window_size_y = 1080;

bool vsync_enabled = true;
bool wireframe_enabled = false;

const char* vox_file_path = "../res/vox/sphere.vox";
MeshingAlgorithm algorithm = slicing;
uint16_t meshing_iterations = 1000;

int main() {
    Renderer::getInstance().configure_algorithm(vox_file_path, algorithm, meshing_iterations);
    Renderer::getInstance().init(window_size_x, window_size_y, vsync_enabled, wireframe_enabled);

    Renderer::getInstance().loop();

	return 0;
}