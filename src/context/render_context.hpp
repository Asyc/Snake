#ifndef SNAKE_RENDER_CONTEXT_HPP
#define SNAKE_RENDER_CONTEXT_HPP

#include <cstdint>
#include <functional>
#include <string_view>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "render/command.hpp"
#include "window/swapchain.hpp"
#include "window/window.hpp"

struct CommandPool;

class RenderContext {
public:
    using PhysicalDeviceSelector = std::function<vk::PhysicalDevice(const std::vector<vk::PhysicalDevice>&)>;

    struct Version {
        uint8_t m_Major, m_Minor, m_Patch;
    };

    struct Queue {
        vk::Queue m_Handle;
        uint32_t m_Index;

        [[nodiscard]] const vk::Queue* operator->() const;
    };

    static vk::PhysicalDevice selectPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);

    RenderContext(Window& window, const std::string_view& title, const Version& version, const PhysicalDeviceSelector& physicalDeviceSelector = selectPhysicalDevice);

    [[nodiscard]] CommandPool createCommandPool(const vk::CommandPoolCreateFlags& flags = {});
    [[nodiscard]] vk::SurfaceKHR getSurface() const;
    [[nodiscard]] vk::PhysicalDevice getPhysicalDevice() const;
    [[nodiscard]] vk::Device getDevice() const;
    [[nodiscard]] const Queue& getGraphicsQueue() const;
    [[nodiscard]] const Queue& getPresentQueue() const;
    [[nodiscard]] Swapchain& getSwapchain();
    [[nodiscard]] const Swapchain& getSwapchain() const;

private:
    vk::UniqueInstance m_Instance;
    vk::UniqueSurfaceKHR m_Surface;

    vk::PhysicalDevice m_PhysicalDevice;
    vk::UniqueDevice m_Device;
    Queue m_GraphicsQueue;
    Queue m_PresentQueue;

    Swapchain m_Swapchain;
};

#endif //SNAKE_RENDER_CONTEXT_HPP
