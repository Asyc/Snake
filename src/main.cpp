#include <chrono>
#include <iostream>
#include <thread>

#include "context/render_context.hpp"
#include "render/pipeline/pipeline_builder.hpp"
#include "window/window.hpp"

void run() {
    Window::initLibrary();
    Window window(1920, 1080, "Snake-Game");
    RenderContext context(window, "Snake", {0, 1, 0});

    PipelineBuilder<0, 0, 0, 0, 0> builder(context, ShaderResource("test"));
    auto pipeline = builder.build();

    auto pool = context.createCommandPool();
    auto buffer = pool.allocateCommandBuffer();

    buffer.begin();
    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());
    buffer.setViewportScissor();
    buffer->draw(3, 1, 0, 0);
    buffer.end();

    auto last = std::chrono::steady_clock::now();
    size_t updates = 0;

    while (!window.shouldClose()) {
        Window::pollEvents();

        auto& image = context.getSwapchain().nextImage();
        image.m_CommandBuffer->executeCommands(1, &*buffer);
        context.getSwapchain().presentImage();

        updates++;
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last).count() >= 1) {
            std::cout << "Updates: " << updates << '\n';
            updates = 0;
            last = now;
        }
    }

    context.getDevice().waitIdle();

    Window::terminateLibrary();
}

int main() {
    try {
        run();
    } catch (const std::exception& ex) {
        std::cerr  << ex.what() << std::endl;
    }
}