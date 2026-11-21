//
// Created by Nemesis Verstraete on 03/11/2026.
//

#include "VulkanPass.h"

#include "Pass.h"
#include "VulkanCommandBuffer.h"

namespace rhi::vulkan {

void VulkanRenderPass::begin(const renderium::RenderPassDescriptor &descriptor) {

}

void VulkanRenderPass::setPipeline(const VulkanRenderPipeline &pipeline) {
}

void VulkanRenderPass::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
    uint32_t firstInstance) {

}

void VulkanRenderPass::end() {
}

}
