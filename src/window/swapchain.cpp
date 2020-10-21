#include "swapchain.hpp"

#include <algorithm>

#include "context/render_context.hpp"

Swapchain::Swapchain(RenderContext* context) : m_Parent(context), m_PresentMode(vk::PresentModeKHR::eImmediate), m_ImageInFlight(0) {
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = context->getPhysicalDevice().getSurfaceFormatsKHR(
            context->getSurface());

    m_SurfaceFormat = surfaceFormats[0];
    for (const auto& format : surfaceFormats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            m_SurfaceFormat = format;
            break;
        }
    }

    vk::AttachmentDescription attachmentDescription({}, m_SurfaceFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorAttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpassDescription(
            {},
            vk::PipelineBindPoint::eGraphics,
            {},
            {},
            1,
            &colorAttachmentReference
    );

    vk::SubpassDependency subpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo renderPassCreateInfo({}, 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency);
    m_RenderPass = m_Parent->getDevice().createRenderPassUnique(renderPassCreateInfo);

    createSwapchain();

    vk::CommandPoolCreateInfo commandPoolCreateInfo({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, m_Parent->getGraphicsQueue().m_Index);
    m_CommandPool = m_Parent->getDevice().createCommandPoolUnique(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(*m_CommandPool, vk::CommandBufferLevel::ePrimary,
                                                            MAX_CONCURRENT_FRAMES);

    vk::CommandBuffer buffers[MAX_CONCURRENT_FRAMES];
    m_Parent->getDevice().allocateCommandBuffers(&commandBufferAllocateInfo, buffers);

    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    vk::FenceCreateInfo fenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < MAX_CONCURRENT_FRAMES; i++) {
        m_ImageFlightData[i] = ImageFlightData{
                vk::UniqueCommandBuffer(buffers[i],
                                        vk::PoolFree<vk::Device, vk::CommandPool, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(
                                                m_Parent->getDevice(), *m_CommandPool)),
                m_Parent->getDevice().createSemaphoreUnique(semaphoreCreateInfo),
                m_Parent->getDevice().createSemaphoreUnique(semaphoreCreateInfo),
                m_Parent->getDevice().createFenceUnique(fenceCreateInfo),
                nullptr
        };
    }
}

void Swapchain::createSwapchain() {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_Parent->getPhysicalDevice().getSurfaceCapabilitiesKHR(
            m_Parent->getSurface());

    m_Extent = surfaceCapabilities.currentExtent;

    std::array<uint32_t, 2> queues = {m_Parent->getGraphicsQueue().m_Index, m_Parent->getPresentQueue().m_Index};
    vk::SharingMode sharingMode = queues[0] == queues[1] ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
            {},
            m_Parent->getSurface(),
            std::clamp(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount,
                       surfaceCapabilities.maxImageCount),
            m_SurfaceFormat.format,
            m_SurfaceFormat.colorSpace,
            m_Extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            sharingMode,
            queues,
            surfaceCapabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            m_PresentMode,
            VK_TRUE,
            *m_Swapchain
    );

    m_Swapchain = m_Parent->getDevice().createSwapchainKHRUnique(swapchainCreateInfo);

    m_Images.resize(swapchainCreateInfo.minImageCount);

    std::vector<vk::Image> images = m_Parent->getDevice().getSwapchainImagesKHR(*m_Swapchain);

    for (uint32_t i = 0; i < m_Images.size(); i++) {
        vk::ImageViewCreateInfo imageViewCreateInfo(
                {},
                images[i],
                vk::ImageViewType::e2D,
                m_SurfaceFormat.format,
                {},
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
        );

        vk::UniqueImageView imageView = m_Parent->getDevice().createImageViewUnique(imageViewCreateInfo);
        vk::FramebufferCreateInfo framebufferCreateInfo({}, *m_RenderPass, 1, &*imageView, m_Extent.width,
                                                        m_Extent.height, 1);
        vk::UniqueFramebuffer framebuffer = m_Parent->getDevice().createFramebufferUnique(framebufferCreateInfo);
        m_Images[i] = Image{
                std::move(imageView),
                std::move(framebuffer),
                i
        };
    }
}

const Swapchain::ImageFlightData& Swapchain::nextImage() {
    if (++m_ImageInFlight >= MAX_CONCURRENT_FRAMES) m_ImageInFlight = 0;
    auto& image = m_ImageFlightData[m_ImageInFlight];

    m_Parent->getDevice().waitForFences(1, &*image.m_InFlight, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = m_Parent->getDevice().acquireNextImageKHR(*m_Swapchain, UINT64_MAX, *image.m_ImageReady, {});
    image.m_BoundImage = &m_Images[imageIndex];

    vk::ClearValue clearValue(vk::ClearColorValue(std::array<float, 4>{}));

    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    image.m_CommandBuffer->begin(commandBufferBeginInfo);

    vk::RenderPassBeginInfo renderPassBeginInfo(*m_RenderPass, *image.m_BoundImage->m_FrameBuffer, vk::Rect2D({}, m_Extent), 1, &clearValue);
    image.m_CommandBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    return image;
}

void Swapchain::presentImage() {
    auto& image = m_ImageFlightData[m_ImageInFlight];

    m_Parent->getDevice().resetFences(1, &*image.m_InFlight);

    image.m_CommandBuffer->endRenderPass();
    image.m_CommandBuffer->end();

    vk::PipelineStageFlags flags(vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::SubmitInfo submitInfo(1, &*image.m_ImageReady, &flags, 1, &*image.m_CommandBuffer, 1, &*image.m_RenderFinished);
    m_Parent->getGraphicsQueue()->submit(1, &submitInfo, *image.m_InFlight);

    vk::PresentInfoKHR presentInfo(1, &*image.m_RenderFinished, 1, &*m_Swapchain, &image.m_BoundImage->m_Index);
    m_Parent->getPresentQueue()->presentKHR(presentInfo);
}

vk::RenderPass Swapchain::getRenderPass() const {
    return *m_RenderPass;
}

vk::Extent2D Swapchain::getExtent() const {
    return m_Extent;
}
