#include "pipeline_builder.hpp"

#include <fstream>
#include <string_view>

inline std::vector<char> readFileData(const std::string_view& path) {
    std::ifstream stream(path.data(), std::ios_base::ate | std::ios_base::binary);
    auto size = stream.tellg();
    stream.seekg(0);

    std::vector<char> buffer(size);
    stream.read(buffer.data(), buffer.size());
    return std::move(buffer);
}

ShaderResource::ShaderResource(std::string name) : m_Name(std::move(name)) {}

void ShaderResource::read(const RenderContext& context) {
    std::vector<char> vertexBuffer = readFileData("assets/shaders/" + m_Name + ".vert.spv");
    std::vector<char> fragmentBuffer = readFileData("assets/shaders/" + m_Name + ".frag.spv");
    vk::ShaderModuleCreateInfo vertexShaderModuleCreateInfo({}, vertexBuffer.size(), reinterpret_cast<uint32_t*>(vertexBuffer.data()));
    vk::ShaderModuleCreateInfo fragmentShaderModuleCreateInfo({}, fragmentBuffer.size(), reinterpret_cast<uint32_t*>(fragmentBuffer.data()));
    m_Vertex = context.getDevice().createShaderModuleUnique(vertexShaderModuleCreateInfo);
    m_Fragment = context.getDevice().createShaderModuleUnique(fragmentShaderModuleCreateInfo);
}
