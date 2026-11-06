//
// Created by Nemesis Verstraete on 03/11/2026.
//

#include "VulkanPass.h"

namespace rhi::vulkan {

void VulkanDynamicRenderPass::begin(const renderium::RenderPassDescriptor &descriptor) {

}

VulkanDynamicRenderPass::VulkanRenderPassResult VulkanDynamicRenderPass::create(
    std::shared_ptr<VulkanCommandEncoder> commandEncoder, const renderium::RenderPassDescriptor &descriptor) {
    auto pass = VulkanDynamicRenderPass(std::move(commandEncoder));
    pass.begin(descriptor);
}

}