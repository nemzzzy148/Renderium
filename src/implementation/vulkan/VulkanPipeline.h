//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../Error.h"

namespace renderium {
struct RenderPipelineCreateInfo;
}

namespace rhi::vulkan {

class VulkanRenderPipeline {
public:
    using PipelineResult = renderium::Result<VulkanRenderPipeline, VulkanError>;
    static PipelineResult create(const renderium::RenderPipelineCreateInfo& createInfo);
private:
    vk::raii::Pipeline pipeline = nullptr;
};

class VulkanComputePipeline {

};

}
