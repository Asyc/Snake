#ifndef SNAKE_COMMAND_HPP
#define SNAKE_COMMAND_HPP

#include <vulkan/vulkan.hpp>

class RenderContext;

struct CommandBuffer {
    RenderContext& m_Parent;
    vk::UniqueCommandBuffer m_Handle;
    vk::CommandBuffer* operator->();
    vk::CommandBuffer& operator*();
    void begin();
    void setViewportScissor();
    void end();
};

struct CommandPool {
    RenderContext& m_Parent;
    vk::UniqueCommandPool m_Handle;
    CommandBuffer allocateCommandBuffer();
};


#endif //SNAKE_COMMAND_HPP
