#ifndef SNAKE_PIPELINE_BUILDER_HPP
#define SNAKE_PIPELINE_BUILDER_HPP

#include <array>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "context/render_context.hpp"
#include "pipeline.hpp"

class ShaderResource {
public:
    explicit ShaderResource(std::string name);
    void read(const RenderContext& context);
    std::string m_Name;
    vk::UniqueShaderModule m_Vertex;
    vk::UniqueShaderModule m_Fragment;
};

template<uint32_t BINDINGS, uint32_t ATTRIBUTES, uint32_t LAYOUTS, uint32_t PUSH_CONSTANTS, uint32_t DYNAMIC_STATES>
class PipelineBuilder {
public:
    PipelineBuilder(const RenderContext& context, ShaderResource shaders)
            : m_Device(context.getDevice()),
              m_ShaderHandles([&context, &shaders](){
                  shaders.read(context);
                  return std::move(shaders);
              }()),
              m_ShaderStages({vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex,
                                                                *m_ShaderHandles.m_Vertex,
                                                                "main", nullptr),
                              vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment,
                                                                *m_ShaderHandles.m_Fragment, "main", nullptr)
                             }),
              m_VertexInputStateCreateInfo({}, BINDINGS, m_Bindings.data(), ATTRIBUTES, m_Attributes.data()),
              m_InputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE),
              m_ViewportStateCreateInfo({}, 1, nullptr, 1, nullptr),
              m_RasterizationStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
                                             vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, VK_FALSE,
                                             0.0f, 0.0f, 0.0f, 1.0f),
              m_MultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 0.0f, nullptr, VK_FALSE),
              m_DepthStencilStateCreateInfo(),
              m_ColorBlendAttachmentState(VK_TRUE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha,
                                          vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
                                          vk::BlendOp::eAdd,
                                          {vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                           vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}),

              m_ColorBlendStateCreateInfo({}, VK_FALSE, vk::LogicOp::eClear, 1, &m_ColorBlendAttachmentState),
              m_DynamicStates(std::array<vk::DynamicState, DYNAMIC_STATES + 2>()),
              m_DynamicStateCreateInfo({}, static_cast<uint32_t>(m_DynamicStates.size()), m_DynamicStates.data()),
              m_LayoutIndex(0),
              m_LayoutCreateInfo({}, static_cast<uint32_t>(m_Layouts.size()), m_Layouts.data(),
                                 static_cast<uint32_t>(m_PushConstants.size()),
                                 m_PushConstants.data()),

              m_PipelineCreateInfo({}, static_cast<uint32_t>(m_ShaderStages.size()), m_ShaderStages.data(),
                                   &m_VertexInputStateCreateInfo,
                                   &m_InputAssemblyStateCreateInfo, nullptr, &m_ViewportStateCreateInfo,
                                   &m_RasterizationStateCreateInfo, &m_MultisampleStateCreateInfo,
                                   &m_DepthStencilStateCreateInfo, &m_ColorBlendStateCreateInfo,
                                   &m_DynamicStateCreateInfo, {}, context.getSwapchain().getRenderPass(), 0, {}, 0) {


    }

    PipelineBuilder& setBinding(uint32_t binding, uint32_t stride) {
        m_Bindings[binding] = vk::VertexInputBindingDescription(binding, stride, vk::VertexInputRate::eVertex);
        return *this;
    }

    PipelineBuilder& setAttribute(uint32_t binding, uint32_t location, vk::Format type, uint32_t offset) {
        m_Attributes[location] = vk::VertexInputAttributeDescription(location, binding, type, offset);
        return *this;
    }

    Pipeline build() {
        vk::UniquePipelineLayout layout = m_Device.createPipelineLayoutUnique(m_LayoutCreateInfo);
        m_PipelineCreateInfo.layout = *layout;

        m_DynamicStates[m_DynamicStates.size() - 1] = vk::DynamicState::eViewport;
        m_DynamicStates[m_DynamicStates.size() - 2] = vk::DynamicState::eScissor;


        return std::move(Pipeline(m_Device.createGraphicsPipelineUnique({}, m_PipelineCreateInfo),
                                  std::move(layout)));
    }

private:
    vk::Device m_Device;

    ShaderResource m_ShaderHandles;
    std::array<vk::PipelineShaderStageCreateInfo, 2> m_ShaderStages;

    vk::PipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo;
    std::array<vk::VertexInputBindingDescription, BINDINGS> m_Bindings;
    std::array<vk::VertexInputAttributeDescription, ATTRIBUTES> m_Attributes;

    vk::PipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo;
    vk::PipelineViewportStateCreateInfo m_ViewportStateCreateInfo;
    vk::PipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo;

    vk::PipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo;
    vk::PipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo;

    vk::PipelineColorBlendAttachmentState m_ColorBlendAttachmentState;
    vk::PipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo;

    std::array<vk::DynamicState, DYNAMIC_STATES + 2> m_DynamicStates;
    vk::PipelineDynamicStateCreateInfo m_DynamicStateCreateInfo;

    size_t m_LayoutIndex;
    std::array<vk::DescriptorSetLayout, LAYOUTS> m_Layouts;
    std::array<vk::PushConstantRange, PUSH_CONSTANTS> m_PushConstants;
    vk::PipelineLayoutCreateInfo m_LayoutCreateInfo;

    vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo;
};


#endif //SNAKE_PIPELINE_BUILDER_HPP
