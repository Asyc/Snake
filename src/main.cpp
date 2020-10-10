#include <iostream>

#include "context/render_context.hpp"
#include "render/pipeline/pipeline_builder.hpp"
#include "window/window.hpp"

int main() {
    Window::initLibrary();
    Window window(1920, 1080, "Snake-Game");
    RenderContext context(window, "Snake", {0, 1, 0});

    PipelineBuilder<0, 0, 0, 0, 0> builder(context, ShaderResource(context, "test"));
    auto pipeline = builder.build();

    vk::CommandPoolCreateInfo commandPoolCreateInfo({}, context.getGraphicsQueue().m_Index);
    vk::UniqueCommandPool pool = context.getDevice().createCommandPoolUnique(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(*pool, vk::CommandBufferLevel::eSecondary, 1);
    vk::UniqueCommandBuffer buffer = std::move(context.getDevice().allocateCommandBuffersUnique(commandBufferAllocateInfo)[0]);

    vk::CommandBufferInheritanceInfo inheritanceInfo(context.getSwapchain().getRenderPass(), 0);
    vk::Viewport viewport(0.0f, 0.0f, context.getSwapchain().getExtent().width, context.getSwapchain().getExtent().height, 0.0f, 1.0f);
    vk::Rect2D scissor({}, context.getSwapchain().getExtent());

    buffer->begin(vk::CommandBufferBeginInfo({vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue}, &inheritanceInfo));
    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());
    buffer->setViewport(0, 1, &viewport);
    buffer->setScissor(0, 1, &scissor);
    buffer->draw(3, 1, 0, 0);
    buffer->end();

    while (!window.shouldClose()) {
        auto& image = context.getSwapchain().nextImage();
        image.m_CommandBuffer->executeCommands(1, &*buffer);
        context.getSwapchain().presentImage();

        Window::pollEvents();
    }

    context.getDevice().waitIdle();

    Window::terminateLibrary();
}