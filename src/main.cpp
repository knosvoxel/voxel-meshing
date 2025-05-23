#include "renderer.h"

int main() {
	
    Renderer::getInstance().init(1920, 1080, true, false);

    Renderer::getInstance().loop();

	return 0;
}