//
// Created by Nemesis Verstraete on 03/11/2026.
//

#include "VulkanPass.h"

#include "Pass.h"
#include "VulkanCommandEncoder.h"

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

VulkanRenderPass::VulkanRenderPassResult VulkanRenderPass::create(
    std::shared_ptr<VulkanCommandEncoder> commandEncoder, const renderium::RenderPassDescriptor &descriptor) {
    auto commandBufferResult = commandEncoder->getAvailableCommandBuffer();
    if (!commandBufferResult.isOk()) {
        VulkanRenderPassResult::err(commandBufferResult.unwrapError());
    }
    auto pass = VulkanRenderPass(std::move(commandEncoder),
        std::move(commandBufferResult.unwrap()));
    pass.begin(descriptor);
    return VulkanRenderPassResult::ok(std::move(pass));
}

}
