#include <chrono>
#include <iostream>
#include <thread>

#include "context/render_context.hpp"
#include "render/pipeline/pipeline_builder.hpp"
#include "window/window.hpp"

struct VertexTest {
    float x, y, z;
    float r, g, b, a;
};

void run() {
    Window::initLibrary();
    Window window(1920, 1080, "Snake-Game");
    RenderContext context(window, "Snake", {0, 1, 0});

    PipelineBuilder<1, 2, 0, 0, 0> builder(context, ShaderResource("standard"));
    builder.setBinding(0, sizeof(VertexTest));
    builder.setAttribute(0, 0, vk::Format::eR32G32B32Sfloat, 0);
    builder.setAttribute(0, 1, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexTest, r));

    auto pipeline = builder.build();

    auto pool = context.createCommandPool();
    auto buffer = pool.allocateCommandBuffer();

    auto vertexBuffer = context.createVertexBuffer(sizeof(VertexTest) * 3);
    auto data = reinterpret_cast<VertexTest*>(vertexBuffer.map());
    data[0] = VertexTest{-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    data[1] = VertexTest{0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    data[2] = VertexTest{0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    vertexBuffer.unmap();


    buffer.begin();
    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());
    buffer.setViewportScissor();
    vk::DeviceSize offsets = 0;
    buffer->bindVertexBuffers(0, 1, &*vertexBuffer, &offsets);
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
#ifdef NDEBUG
    try {
        run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what();
    }
#else
    run();
#endif
}