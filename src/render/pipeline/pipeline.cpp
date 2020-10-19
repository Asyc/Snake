#include "pipeline.hpp"

Pipeline::Pipeline(vk::UniquePipeline pipeline, vk::UniquePipelineLayout layout) : m_Pipeline(std::move(pipeline)),
                                                                                   m_Layout(std::move(layout)) {}

vk::Pipeline Pipeline::getPipeline() const {
    return *m_Pipeline;
}

vk::PipelineLayout Pipeline::getLayout() const {
    return *m_Layout;
}
