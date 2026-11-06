//
// Created by Nemesis Verstraete on 03/11/2026.
//

#pragma once
#include <memory>

#include "Result.h"
#include "vulkan/vulkan_raii.hpp"

namespace renderium {
struct RenderPassDescriptor;
}

namespace rhi::vulkan {
class VulkanRenderPipeline;
enum class VulkanError;
class VulkanCommandEncoder;

class VulkanRenderPass {
    explicit VulkanRenderPass(std::shared_ptr<VulkanCommandEncoder> commandEncoder,
        std::shared_ptr<vk::raii::CommandBuffer> commandBuffer) : commandBuffer(std::move(commandBuffer)),
        commandEncoder(std::move(commandEncoder)) {}

    void begin(const renderium::RenderPassDescriptor& descriptor);

    void setPipeline(const VulkanRenderPipeline& pipeline);
    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);

    void end();
public:
    using VulkanRenderPassResult = renderium::Result<VulkanRenderPass, VulkanError>;
    static VulkanRenderPassResult create(std::shared_ptr<VulkanCommandEncoder> commandEncoder,
        const renderium::RenderPassDescriptor& descriptor);
private:
    std::shared_ptr<vk::raii::CommandBuffer> commandBuffer;
    std::shared_ptr<VulkanCommandEncoder> commandEncoder;
};

}