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
    const vk::ShaderModuleCreateInfo createInfo {
        .codeSize = entryPoint.code.size() * sizeof(uint8_t),
        .pCode = reinterpret_cast<uint32_t const *>(entryPoint.code.data())
    };
    return {device, createInfo};
}

template<IsState State>
std::optional<vk::PipelineShaderStageCreateInfo> VulkanRenderPipeline::createShaderStage(
        const vk::raii::Device& device, const State& state, vk::raii::ShaderModule& shaderModule) {
    const auto* specificState = findEntryPoint(
        state.module.getCompiledShader().entryPoints, state.entryPoint);
    if (!specificState) return std::nullopt;
    shaderModule = createShaderModule(device, *specificState);

    vk::PipelineShaderStageCreateInfo vertexInfo {
        .stage = VulkanShaderStage_t<State>,
        .module = shaderModule,
        .pName = specificState->compiledName.c_str()
    };
    return vertexInfo;
}

size_t VulkanRenderPipeline::totalVertexAttributeCount(const renderium::VertexState &vertexState) {
    size_t size = 0;
    for (const auto& layout : vertexState.buffers) {
        size += layout.attributes.size();
    }
    return size;
}

std::vector<vk::VertexInputBindingDescription> VulkanRenderPipeline::createVertexBindings(
    const renderium::VertexState &vertexState) {
    std::vector<vk::VertexInputBindingDescription> bindings;
    bindings.reserve(vertexState.buffers.size());
    for (size_t slot = 0; slot < vertexState.buffers.size(); ++slot) {
        const auto& layout = vertexState.buffers[slot];
        bindings.emplace_back(static_cast<uint32_t>(slot), layout.arrayStride,
            VulkanConversion::mapVertexStepMode(layout.stepMode));
    }
    return std::move(bindings);
}

std::vector<vk::VertexInputAttributeDescription> VulkanRenderPipeline::createVertexAttributes(
    const renderium::VertexState &vertexState) {
    std::vector<vk::VertexInputAttributeDescription> attributes;
    attributes.reserve(totalVertexAttributeCount(vertexState));

    for (size_t slot = 0; slot < vertexState.buffers.size(); ++slot) {
        const auto& layout = vertexState.buffers[slot];
        for (const auto& attribute : layout.attributes) {
            attributes.emplace_back(attribute.shaderLocation, static_cast<uint32_t>(slot),
                VulkanConversion::mapVertexFormat(attribute.format), attribute.offset);
        }
    }

    return std::move(attributes);
}

vk::PipelineVertexInputStateCreateInfo VulkanRenderPipeline::createVertexInputInfo(
    const renderium::VertexState &vertexState, std::vector<vk::VertexInputBindingDescription>& bindings,
    std::vector<vk::VertexInputAttributeDescription>& attributes) {
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
    const renderium::FragmentState &fragmentState, std::vector<vk::PipelineColorBlendAttachmentState>& attachments) {
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

vk::PipelineRenderingCreateInfo VulkanRenderPipeline::createPipelineRenderingInfo(
    const renderium::FragmentState &fragmentState, std::vector<vk::Format> &attachmentFormats) {
    attachmentFormats.reserve(fragmentState.targets.size());
    for (const auto&[format]: fragmentState.targets) {
        attachmentFormats.push_back(VulkanConversion::mapTextureFormat(format));
    }
    return {
        .colorAttachmentCount = static_cast<uint32_t>(attachmentFormats.size()),
        .pColorAttachmentFormats = attachmentFormats.data()
    };
}

VulkanRenderPipeline::PipelineResult
VulkanRenderPipeline::create(const VulkanDevice& device, const renderium::RenderPipelineDescriptor& descriptor) {

    // vertex & fragment shader stages
    vk::raii::ShaderModule vertexShaderModule = nullptr;
    const auto vertexInfo = createShaderStage(device.getDevice(),
        descriptor.vertex,vertexShaderModule);
    if (!vertexInfo) return PipelineResult::err(VulkanError::VertexEntryPointNotFound);

    vk::raii::ShaderModule fragmentShaderModule = nullptr;
    const auto fragmentInfo = createShaderStage(device.getDevice(),
        descriptor.fragment, fragmentShaderModule);
    if (!fragmentInfo) return PipelineResult::err(VulkanError::FragmentEntryPointNotFound);

    vk::PipelineShaderStageCreateInfo shaderStages[] {
        vertexInfo.value(), fragmentInfo.value()
    };
    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributes;
    auto vertexInputInfo = createVertexInputInfo(descriptor.vertex, bindings, attributes);

    // topology & viewport
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
        .topology = VulkanConversion::mapPrimitiveTopology(descriptor.primitive.topology)};
    vk::PipelineViewportStateCreateInfo viewportState { .viewportCount = 1, .scissorCount = 1 };

    // rasterizer
    auto rasterizer = createRasterizer(descriptor.primitive);
    vk::PipelineMultisampleStateCreateInfo multisampling {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    // color blend attachments
    std::vector<vk::PipelineColorBlendAttachmentState> blendAttachment;
    auto colorBlending = createColorBlendAttachments(descriptor.fragment, blendAttachment);

    // dynamic states
    std::vector dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    // layout
    vk::PipelineLayoutCreateInfo layoutInfo;
    auto layout = vk::raii::PipelineLayout(device.getDevice(), layoutInfo);

    std::vector<vk::Format> attachmentFormats;
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
            .layout = layout,
            .renderPass = nullptr
        }, createPipelineRenderingInfo(descriptor.fragment, attachmentFormats)
    };

    return PipelineResult::ok(VulkanRenderPipeline(vk::raii::Pipeline(device.getDevice(), nullptr,
        pipelineCreateInfo.get<vk::GraphicsPipelineCreateInfo>())));
}

}