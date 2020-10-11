#include "command.hpp"

#include "context/render_context.hpp"

vk::CommandBuffer* CommandBuffer::operator->() {
    return m_Handle.operator->();
}

vk::CommandBuffer& CommandBuffer::operator*() {
    return *m_Handle;
}

void CommandBuffer::begin() {
    using Flags = vk::CommandBufferUsageFlagBits;
    vk::CommandBufferInheritanceInfo inheritanceInfo(m_Parent.getSwapchain().getRenderPass(), 0);
    m_Handle->begin(vk::CommandBufferBeginInfo({Flags::eSimultaneousUse | Flags::eRenderPassContinue}, &inheritanceInfo));
}

void CommandBuffer::setViewportScissor() {
    vk::Rect2D scissor({}, m_Parent.getSwapchain().getExtent());
    vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(scissor.extent.width), static_cast<float>(scissor.extent.height));

    m_Handle->setViewport(0, 1, &viewport);
    m_Handle->setScissor(0, 1, &scissor);
}

void CommandBuffer::end() {
    m_Handle->end();
}

CommandBuffer CommandPool::allocateCommandBuffer() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(*m_Handle, vk::CommandBufferLevel::eSecondary, 1);
    return {m_Parent, std::move(m_Parent.getDevice().allocateCommandBuffersUnique(commandBufferAllocateInfo)[0])};
}