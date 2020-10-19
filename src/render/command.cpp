#include "command.hpp"

#include "context/render_context.hpp"

const vk::CommandBuffer* CommandBuffer::operator->() const {
    return &m_Handle;
}

const vk::CommandBuffer& CommandBuffer::operator*() const {
    return m_Handle;
}

void CommandBuffer::begin() const {
    m_Handle.begin(vk::CommandBufferBeginInfo());
}

void CommandBuffer::setViewportScissor() const {
    vk::Rect2D scissor({}, m_Parent.getSwapchain().getExtent());
    vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(scissor.extent.width),
                          static_cast<float>(scissor.extent.height));

    m_Handle.setViewport(0, 1, &viewport);
    m_Handle.setScissor(0, 1, &scissor);
}

void CommandBuffer::bindVertexBuffer(const VertexBuffer& vertexBuffer) const {
    std::array<vk::DeviceSize, 1> sizes{};
    m_Handle.bindVertexBuffers(0, vertexBuffer.getBuffer(), sizes);

    m_Handle.bindIndexBuffer(vertexBuffer.getIndexBuffer(), 0, vk::IndexType::eUint32);
}

void CommandBuffer::drawVertexBuffer(const VertexBuffer& vertexBuffer) const {
    m_Handle.drawIndexed(vertexBuffer.getIndices(), 1, 0, 0, 0);
}

void CommandBuffer::end() const {
    m_Handle.end();
}
