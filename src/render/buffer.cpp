#include "buffer.hpp"

#include <optional>

#include "context/render_context.hpp"

VertexBuffer::VertexBuffer(RenderContext& context, size_t size, size_t indexCount) : m_Parent(context), m_IndexCount(
        static_cast<uint32_t>(indexCount)) {
    vk::BufferCreateInfo bufferCreateInfo({}, size, {vk::BufferUsageFlagBits::eVertexBuffer},
                                          vk::SharingMode::eExclusive);

    vk::BufferCreateInfo indexBufferCreateInfo({}, sizeof(uint32_t) * indexCount, vk::BufferUsageFlagBits::eIndexBuffer,
                                               vk::SharingMode::eExclusive);

    m_Buffer = context.getDevice().createBufferUnique(bufferCreateInfo);
    m_IndexBuffer = context.getDevice().createBufferUnique(indexBufferCreateInfo);

    vk::MemoryRequirements memoryRequirements = context.getDevice().getBufferMemoryRequirements(*m_Buffer);
    vk::MemoryRequirements indexBufferMemoryRequirements = context.getDevice().getBufferMemoryRequirements(
            *m_IndexBuffer);

    vk::PhysicalDeviceMemoryProperties memoryProperties = context.getPhysicalDevice().getMemoryProperties();

    std::optional<uint32_t> vertexBufferIndex, indexBufferIndex;
    uint32_t index = 0;
    for (const auto& memoryProperty : memoryProperties.memoryTypes) {
        using Flag = vk::MemoryPropertyFlagBits;

        if (memoryProperty.propertyFlags & Flag::eHostCoherent && memoryProperty.propertyFlags &&
            memoryProperty.propertyFlags & Flag::eHostVisible) {
            if (!vertexBufferIndex.has_value() &&
                memoryProperty.propertyFlags & vk::MemoryPropertyFlags(memoryRequirements.memoryTypeBits)) {
                vertexBufferIndex = index;
            }

            if (!indexBufferIndex.has_value() &&
                memoryProperty.propertyFlags & vk::MemoryPropertyFlags(indexBufferMemoryRequirements.memoryTypeBits)) {
                indexBufferIndex = index;
            }

            if (vertexBufferIndex.has_value() && indexBufferIndex.has_value()) break;
        }

        index++;
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size, vertexBufferIndex.value());
    m_DeviceMemory = context.getDevice().allocateMemoryUnique(memoryAllocateInfo);
    context.getDevice().bindBufferMemory(*m_Buffer, *m_DeviceMemory, 0);

    vk::MemoryAllocateInfo indexBufferMemoryAllocateInfo(indexBufferMemoryRequirements.size, indexBufferIndex.value());
    m_IndexDeviceMemory = context.getDevice().allocateMemoryUnique(indexBufferMemoryAllocateInfo);
    context.getDevice().bindBufferMemory(*m_IndexBuffer, *m_IndexDeviceMemory, 0);
}

const vk::Buffer& VertexBuffer::operator*() const {
    return *m_Buffer;
}

void* VertexBuffer::map() const {
    return m_Parent.getDevice().mapMemory(*m_DeviceMemory, 0, VK_WHOLE_SIZE);
}

void VertexBuffer::unmap() const {
    return m_Parent.getDevice().unmapMemory(*m_DeviceMemory);
}

uint32_t* VertexBuffer::mapIndexBuffer() const {
    return reinterpret_cast<uint32_t*>(m_Parent.getDevice().mapMemory(*m_IndexDeviceMemory, 0, VK_WHOLE_SIZE));
}

void VertexBuffer::unmapIndexBuffer() const {
    m_Parent.getDevice().unmapMemory(*m_IndexDeviceMemory);
}

vk::Buffer VertexBuffer::getBuffer() const {
    return *m_Buffer;
}

vk::Buffer VertexBuffer::getIndexBuffer() const {
    return *m_IndexBuffer;
}

uint32_t VertexBuffer::getIndices() const {
    return m_IndexCount;
}
