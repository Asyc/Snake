#ifndef SNAKE_PIPELINE_HPP
#define SNAKE_PIPELINE_HPP

#include <vulkan/vulkan.hpp>

class Pipeline {
public:
    Pipeline(vk::UniquePipeline pipeline, vk::UniquePipelineLayout layout);
    [[nodiscard]] vk::Pipeline getPipeline() const;
private:
    vk::UniquePipeline m_Pipeline;
    vk::UniquePipelineLayout m_Layout;
};

#endif //SNAKE_PIPELINE_HPP
