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
class VulkanCommandBuffer;
class VulkanRenderPipeline;
enum class VulkanError;

class VulkanRenderPass {
    explicit VulkanRenderPass(std::shared_ptr<VulkanCommandBuffer> commandBuffer,
        std::shared_ptr<vk::raii::CommandBuffer> vulkanBuffer) : vulkanBuffer(std::move(vulkanBuffer)),
        commandBuffer(std::move(commandBuffer)) {}

    void begin(const renderium::RenderPassDescriptor& descriptor);

    void setPipeline(const VulkanRenderPipeline& pipeline);
    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);

    void end();
public:
    using VulkanRenderPassResult = renderium::Result<VulkanRenderPass, VulkanError>;
private:
    std::shared_ptr<vk::raii::CommandBuffer> vulkanBuffer;
    std::shared_ptr<VulkanCommandBuffer> commandBuffer;
};

}