//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanPipeline.h"

#include "Pipeline.h"
#include "ShaderCompiler.h"
#include "VulkanConversion.h"
#include "VulkanDevice.h"
#include "../PipelineImpl.h"

namespace rhi::vulkan {

vk::raii::ShaderModule VulkanRenderPipeline::createShaderModule(const vk::raii::Device& device,
        const renderium::shader::ShaderEntryPoint &entryPoint) {
    vk::ShaderModuleCreateInfo createInfo {
        .codeSize = entryPoint.code.size() * sizeof(uint8_t),
        .pCode = reinterpret_cast<uint32_t const *>(entryPoint.code.data())
    };
    return vk::raii::ShaderModule(device, createInfo);
}

template<IsState State>
std::optional<vk::PipelineShaderStageCreateInfo> VulkanRenderPipeline::createShaderStage(
        const vk::raii::Device& device, const State& state, vk::ShaderModule& shaderModule) {
    const auto* specificState = findEntryPoint(
        state.shader.getCompiledShader().entryPoints, state.functionEntryPointName);
    if (!specificState) return std::nullopt;
    shaderModule = createShaderModule(device, *specificState);

    vk::PipelineShaderStageCreateInfo vertexInfo {
        .stage = VulkanShaderStage_t<State>,
        .module = shaderModule,
        .pName = state.functionEntryPointName.c_str()
    };
    return vertexInfo;
}

size_t VulkanRenderPipeline::totalVertexAttributeCount(const renderium::VertexState &vertexState) {
    size_t size = 0;
    for (const auto& layout : vertexState.layouts) {
        size += layout.attributes.size();
    }
    return size;
}

std::vector<vk::VertexInputBindingDescription> VulkanRenderPipeline::createVertexBindings(
    const renderium::VertexState &vertexState) {
    std::vector<vk::VertexInputBindingDescription> bindings;
    bindings.reserve(vertexState.layouts.size());
    for (const auto& layout : vertexState.layouts) {
        bindings.emplace_back(layout.binding, layout.vertexStride,
            VulkanConversion::mapVertexStepMode(layout.stepMode));
    }
    return std::move(bindings);
}

std::vector<vk::VertexInputAttributeDescription> VulkanRenderPipeline::createVertexAttributes(
    const renderium::VertexState &vertexState) {
    std::vector<vk::VertexInputAttributeDescription> attributes;
    attributes.reserve(totalVertexAttributeCount(vertexState));

    for (const auto& layout : vertexState.layouts) {
        for (const auto&[format, offset, location] : layout.attributes) {
            attributes.emplace_back(location, layout.binding,
                VulkanConversion::mapVertexFormat(format), offset);
        }
    }

    return std::move(attributes);
}

vk::PipelineVertexInputStateCreateInfo VulkanRenderPipeline::createVertexInputInfo(
    const renderium::VertexState &vertexState, std::vector<vk::VertexInputBindingDescription>& bindings,
    std::vector<vk::VertexInputAttributeDescription> attributes) {
    bindings = createVertexBindings(vertexState);
    attributes = createVertexAttributes(vertexState);

    const vk::PipelineVertexInputStateCreateInfo vertexInputInfo {
        .vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
        .pVertexBindingDescriptions = bindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
        .pVertexAttributeDescriptions = attributes.data()
    };

    return vertexInputInfo;
}

vk::PipelineRasterizationStateCreateInfo VulkanRenderPipeline::createRasterizer(
    const renderium::PrimitiveState &primitiveState) {
    return  {
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = VulkanConversion::mapCullMode(primitiveState.cullMode),
        .frontFace = VulkanConversion::mapFrontFace(primitiveState.frontFace),
        .depthBiasClamp = vk::False,
        .lineWidth = 1.0
    };
}

vk::PipelineColorBlendStateCreateInfo VulkanRenderPipeline::createColorBlendAttachments(
    const renderium::FragmentState &fragmentState, std::vector<vk::PipelineColorBlendAttachmentState> attachments) {
    attachments.reserve(fragmentState.targets.size());
    for (const auto& target : fragmentState.targets) {
        attachments.push_back({
            .blendEnable = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        });
    }

    return {
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data()
    };
}

VulkanRenderPipeline::PipelineResult
VulkanRenderPipeline::create(const VulkanDevice& device, const renderium::RenderPipelineCreateInfo& createInfo) {

    // vertex & fragment shader stages
    vk::ShaderModule vertexShaderModule;
    const auto vertexInfo = createShaderStage(device.getDevice(),
        createInfo.vertexState,vertexShaderModule);
    if (!vertexInfo) return PipelineResult::err(VulkanError::VertexEntryPointNotFound);
    vk::ShaderModule fragmentShaderModule;
    const auto fragmentInfo = createShaderStage(device.getDevice(),
        createInfo.fragmentState, fragmentShaderModule);
    if (!fragmentInfo) return PipelineResult::err(VulkanError::FragmentEntryPointNotFound);
    vk::PipelineShaderStageCreateInfo shaderStages[] {
        vertexInfo.value(), fragmentInfo.value()
    };
    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributes;
    auto vertexInputInfo = createVertexInputInfo(createInfo.vertexState, bindings, attributes);

    // topology & viewport
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
        .topology = VulkanConversion::mapPrimitiveTopology(createInfo.primitiveState.primitiveTopology)};
    vk::PipelineViewportStateCreateInfo viewportState { .viewportCount = 1, .scissorCount = 1 };

    // rasterizer
    auto rasterizer = createRasterizer(createInfo.primitiveState);
    vk::PipelineMultisampleStateCreateInfo multisampling {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    // color blend attachments
    std::vector<vk::PipelineColorBlendAttachmentState> attachments;
    auto colorBlending = createColorBlendAttachments(createInfo.fragmentState, attachments);

    // dynamic states
    std::vector dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfo {
        {
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = nullptr,
            .renderPass = nullptr
        }, {
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = vk::Format::eA1B5G5R5UnormPack16
        }
    };

    return PipelineResult::ok(VulkanRenderPipeline(vk::raii::Pipeline(device.getDevice(), nullptr,
        pipelineCreateInfo.get<vk::GraphicsPipelineCreateInfo>())));
}

}