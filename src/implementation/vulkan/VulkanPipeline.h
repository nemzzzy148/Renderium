//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../Error.h"

namespace renderium {
struct PrimitiveState;
enum class CullMode;
enum class FrontFace;
enum class PrimitiveTopology;
enum class VertexFormat;
enum class VertexStepMode;
struct FragmentState;
struct VertexState;

namespace shader {
struct ShaderEntryPoint;
}

struct RenderPipelineDescriptor;
}

namespace rhi::vulkan {
class VulkanDevice;

template<typename State>
concept IsState = std::is_same_v<State, renderium::VertexState> || std::is_same_v<State, renderium::FragmentState>;

template<IsState State>
struct VulkanShaderStageWrapper;
template<> struct VulkanShaderStageWrapper<renderium::VertexState> { static constexpr auto Stage = vk::ShaderStageFlagBits::eVertex; };
template<> struct VulkanShaderStageWrapper<renderium::FragmentState> { static constexpr auto Stage = vk::ShaderStageFlagBits::eFragment; };
template<IsState State>
inline constexpr vk::ShaderStageFlagBits VulkanShaderStage_t = VulkanShaderStageWrapper<State>::Stage;

class VulkanRenderPipeline {
    explicit VulkanRenderPipeline(vk::raii::Pipeline pipeline) : pipeline(std::move(pipeline)) {}
    // shader
    static vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device,
        const renderium::shader::ShaderEntryPoint& entryPoint);
    template<IsState State>
    static std::optional<vk::PipelineShaderStageCreateInfo> createShaderStage(const vk::raii::Device& device,
        const State& state, vk::raii::ShaderModule& shaderModule);

    // vertex state
    static size_t totalVertexAttributeCount(const renderium::VertexState& vertexState);
    static std::vector<vk::VertexInputBindingDescription> createVertexBindings(const renderium::VertexState& vertexState);
    static std::vector<vk::VertexInputAttributeDescription> createVertexAttributes(const renderium::VertexState& vertexState);
    static vk::PipelineVertexInputStateCreateInfo createVertexInputInfo(const renderium::VertexState& vertexState,
        std::vector<vk::VertexInputBindingDescription>& bindings, std::vector<vk::VertexInputAttributeDescription>& attributes);

    // rasterizer
    static vk::PipelineRasterizationStateCreateInfo createRasterizer(const renderium::PrimitiveState& primitiveState);

    // color
    static vk::PipelineColorBlendStateCreateInfo createColorBlendAttachments(const renderium::FragmentState& fragmentState,
        std::vector<vk::PipelineColorBlendAttachmentState>& attachments);
    static vk::PipelineRenderingCreateInfo createPipelineRenderingInfo(const renderium::FragmentState& fragmentState,
        std::vector<vk::Format>& attachmentFormats);
public:
    using PipelineResult = renderium::Result<VulkanRenderPipeline, VulkanError>;
    static PipelineResult create(const VulkanDevice& device, const renderium::RenderPipelineDescriptor& descriptor);
private:
    vk::raii::Pipeline pipeline = nullptr;
};

class VulkanComputePipeline {

};

}
