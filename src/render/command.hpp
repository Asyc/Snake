#ifndef SNAKE_COMMAND_HPP
#define SNAKE_COMMAND_HPP

#include <vulkan/vulkan.hpp>

class RenderContext;

class VertexBuffer;

struct CommandBuffer {
    RenderContext& m_Parent;
    vk::CommandBuffer m_Handle;
    const vk::CommandBuffer* operator->() const;
    const vk::CommandBuffer& operator*() const;
    void begin() const;

    void setViewportScissor() const;

    void bindVertexBuffer(const VertexBuffer& vertexBuffer) const;
    void drawVertexBuffer(const VertexBuffer& vertexBuffer) const;

    void end() const;
};

#endif //SNAKE_COMMAND_HPP
