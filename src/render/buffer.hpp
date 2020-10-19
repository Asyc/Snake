#ifndef SNAKE_BUFFER_HPP
#define SNAKE_BUFFER_HPP

#include <vulkan/vulkan.hpp>

class RenderContext;

class VertexBuffer {
public:
    VertexBuffer(RenderContext& context, size_t size, size_t indexCount);

    const vk::Buffer& operator*() const;

    [[nodiscard]] void* map() const;

    void unmap() const;

    [[nodiscard]] uint32_t* mapIndexBuffer() const;

    void unmapIndexBuffer() const;

    [[nodiscard]] vk::Buffer getBuffer() const;

    [[nodiscard]] vk::Buffer getIndexBuffer() const;

    [[nodiscard]] uint32_t getIndices() const;
private:
    RenderContext& m_Parent;
    vk::UniqueBuffer m_Buffer;
    vk::UniqueDeviceMemory m_DeviceMemory;

    vk::UniqueBuffer m_IndexBuffer;
    vk::UniqueDeviceMemory m_IndexDeviceMemory;

    uint32_t m_IndexCount;
};

#endif //SNAKE_BUFFER_HPP
