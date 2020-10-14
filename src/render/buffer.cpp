#include "buffer.hpp"

#include "context/render_context.hpp"

VertexBuffer::VertexBuffer(RenderContext& context, size_t size) : m_Parent(context) {
    vk::BufferCreateInfo bufferCreateInfo({}, size, {vk::BufferUsageFlagBits::eVertexBuffer},
                                          vk::SharingMode::eExclusive, 1);

    m_Buffer = context.getDevice().createBufferUnique(bufferCreateInfo);

    vk::MemoryRequirements memoryRequirements = context.getDevice().getBufferMemoryRequirements(*m_Buffer);
    vk::PhysicalDeviceMemoryProperties memoryProperties = context.getPhysicalDevice().getMemoryProperties();

    uint32_t index = 0;
    for (const auto& memoryProperty : memoryProperties.memoryTypes) {
        using Flag = vk::MemoryPropertyFlagBits;
        if (memoryProperty.propertyFlags & vk::MemoryPropertyFlags(memoryRequirements.memoryTypeBits) &&
            memoryProperty.propertyFlags & Flag::eHostCoherent && memoryProperty.propertyFlags && memoryProperty.propertyFlags & Flag::eHostVisible) {
            break;
        }

        index++;
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size, index);
    m_DeviceMemory = context.getDevice().allocateMemoryUnique(memoryAllocateInfo);
    context.getDevice().bindBufferMemory(*m_Buffer, *m_DeviceMemory, 0);
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
