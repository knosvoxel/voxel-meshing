#include "renderer.h"

uint16_t window_size_x = 1920;
uint16_t window_size_y = 1080;

bool vsync_enabled = true;
bool wireframe_enabled = false;

int main() {
    Renderer::getInstance().init(window_size_x, window_size_y, vsync_enabled, wireframe_enabled);

    Renderer::getInstance().loop();

	return 0;
}