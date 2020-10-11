#ifndef SNAKE_SWAPCHAIN_HPP
#define SNAKE_SWAPCHAIN_HPP

#include <array>
#include <cstdint>
#include <memory>

#include <vulkan/vulkan.hpp>

class RenderContext;

constexpr size_t MAX_CONCURRENT_FRAMES = 3;

class Swapchain {
public:
    struct Image {
        vk::UniqueImageView m_ImageView;
        vk::UniqueFramebuffer m_FrameBuffer;
        uint32_t m_Index;
    };

    struct ImageFlightData {
        vk::UniqueCommandBuffer m_CommandBuffer;
        vk::UniqueSemaphore m_ImageReady, m_RenderFinished;
        vk::UniqueFence m_InFlight;
        Image* m_BoundImage;
    };

    explicit Swapchain(RenderContext* context);

    void createSwapchain();

    const ImageFlightData& nextImage();
    void presentImage();

    [[nodiscard]] vk::Extent2D getExtent() const;
    [[nodiscard]] vk::RenderPass getRenderPass() const;

private:
    Swapchain() = default;

    RenderContext* m_Parent;
    vk::UniqueSwapchainKHR m_Swapchain;
    vk::SurfaceFormatKHR m_SurfaceFormat;
    vk::Extent2D m_Extent;
    vk::PresentModeKHR m_PresentMode;

    vk::UniqueRenderPass m_RenderPass;
    vk::UniqueCommandPool m_CommandPool;

    std::vector<Image> m_Images;
    std::array<ImageFlightData, MAX_CONCURRENT_FRAMES> m_ImageFlightData;

    size_t m_ImageInFlight;

    friend class RenderContext;
};

#endif //SNAKE_SWAPCHAIN_HPP
