#include <iostream>

#include "context/render_context.hpp"
#include "window/window.hpp"

int main() {
    Window::initLibrary();
    Window window(1920, 1080, "Snake-Game");
    RenderContext context(window, "Snake", {0, 1, 0});

    while (!window.shouldClose()) {
        context.getSwapchain().nextImage();
        context.getSwapchain().presentImage();

        Window::pollEvents();
    }

    context.getDevice().waitIdle();

    Window::terminateLibrary();
}