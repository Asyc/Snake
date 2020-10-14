#ifndef SNAKE_BUFFER_HPP
#define SNAKE_BUFFER_HPP

#include <vulkan/vulkan.hpp>

class RenderContext;

class VertexBuffer {
public:
    VertexBuffer(RenderContext& context, size_t size);

    const vk::Buffer& operator*() const;

    [[nodiscard]] void* map() const;
    void unmap() const;
private:
    RenderContext& m_Parent;
    vk::UniqueBuffer m_Buffer;
    vk::UniqueDeviceMemory m_DeviceMemory;
};

#endif //SNAKE_BUFFER_HPP
