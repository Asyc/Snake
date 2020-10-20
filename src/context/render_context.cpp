#include "render_context.hpp"

#include <iostream>
#include <optional>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

RenderContext::RenderContext(Window& window, const std::string_view& title, const Version& version, const PhysicalDeviceSelector& selector) {
    // Creating Vulkan Instance
    uint32_t vkVersion = VK_MAKE_VERSION(version.m_Major, version.m_Minor, version.m_Patch);
    vk::ApplicationInfo applicationInfo(title.data(), vkVersion, "No-Engine", vkVersion, VK_API_VERSION_1_2);

    uint32_t extensionCount;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo, 0, nullptr, extensionCount, extensions);
#ifndef NDEBUG
    std::array<const char*, 1> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif
    m_Instance = vk::createInstanceUnique(instanceCreateInfo);

    // Creating Window Surface
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(m_Instance->operator VkInstance_T *(), window.getWindowHandle(), nullptr, &surface);
    m_Surface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(surface), vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(*m_Instance));

    m_PhysicalDevice = selector(m_Instance->enumeratePhysicalDevices());

    // Finding Queue Families
    std::optional<uint32_t> graphicsIndex, presentIndex;
    uint32_t index = 0;
    for (const auto& family : m_PhysicalDevice.getQueueFamilyProperties()) {
        if (!graphicsIndex.has_value() && family.queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsIndex = index;
        }

        if (!presentIndex.has_value() && m_PhysicalDevice.getSurfaceSupportKHR(index, *m_Surface)) {
            presentIndex = index;
        }

        if (graphicsIndex.has_value() && presentIndex.has_value()) break;
        index++;
    }

    // Creating the logical device
    uint32_t queueCount = graphicsIndex == presentIndex ? 1 : 2;

    float priority = 1.0f;
    std::array<vk::DeviceQueueCreateInfo, 2> queueCreateInfos = {
            vk::DeviceQueueCreateInfo({}, graphicsIndex.value(), 1, &priority),
            vk::DeviceQueueCreateInfo({}, presentIndex.value(), 1, &priority)
    };


#if !defined(NDEBUG) && !defined(__APPLE__)
    std::array<const char*, 2> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};
#else
    std::array<const char*, 1> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif
    vk::PhysicalDeviceFeatures features;
    vk::DeviceCreateInfo deviceCreateInfo({}, queueCount, queueCreateInfos.data(), 0, nullptr, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), &features);
    m_Device = m_PhysicalDevice.createDeviceUnique(deviceCreateInfo);
    m_GraphicsQueue = {m_Device->getQueue(graphicsIndex.value(), 0), graphicsIndex.value()};
    m_PresentQueue = {m_Device->getQueue(presentIndex.value(), 0), presentIndex.value()};

    m_Swapchain = std::move(Swapchain(this));
}

VertexBuffer RenderContext::createVertexBuffer(size_t size, size_t indexCount) {
    return VertexBuffer(*this, size, indexCount);
}

vk::SurfaceKHR RenderContext::getSurface() const {
    return *m_Surface;
}

vk::PhysicalDevice RenderContext::getPhysicalDevice() const {
    return m_PhysicalDevice;
}

vk::Device RenderContext::getDevice() const {
    return *m_Device;
}

const RenderContext::Queue& RenderContext::getGraphicsQueue() const {
    return m_GraphicsQueue;
}

const RenderContext::Queue& RenderContext::getPresentQueue() const {
    return m_PresentQueue;
}

Swapchain& RenderContext::getSwapchain() {
    return m_Swapchain;
}

const Swapchain& RenderContext::getSwapchain() const {
    return m_Swapchain;
}

vk::PhysicalDevice RenderContext::selectPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices) {
    return devices[0];
}

const vk::Queue* RenderContext::Queue::operator->() const {
    return &m_Handle;
}