//
// Created by Nemesis Verstraete on 03/11/2026.
//

#pragma once
#include <memory>

#include "Result.h"

namespace renderium {
struct RenderPassDescriptor;
}

namespace rhi::vulkan {
enum class VulkanError;
class VulkanCommandEncoder;

class VulkanDynamicRenderPass {
    explicit VulkanDynamicRenderPass(std::shared_ptr<VulkanCommandEncoder> commandEncoder)
        : commandEncoder(std::move(commandEncoder)) {}

    void begin(const renderium::RenderPassDescriptor& descriptor);
    void end();
public:
    using VulkanRenderPassResult = renderium::Result<VulkanDynamicRenderPass, VulkanError>;
    static VulkanRenderPassResult create(std::shared_ptr<VulkanCommandEncoder> commandEncoder,
        const renderium::RenderPassDescriptor& descriptor);
private:
    std::shared_ptr<VulkanCommandEncoder> commandEncoder;
};

}