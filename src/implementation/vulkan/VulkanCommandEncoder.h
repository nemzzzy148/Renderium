//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <Result.h>

namespace rhi::vulkan {
enum class VulkanError;
class VulkanDevice;

class VulkanCommandEncoder {
public:
    using CommandEncoderResult = renderium::Result<std::shared_ptr<VulkanCommandEncoder>, VulkanError>;
    static CommandEncoderResult create(std::shared_ptr<VulkanDevice> device, uint32_t queueFamilyIndex);

    [[nodiscard]] size_t size() const { return commandBuffers.size(); }

    std::optional<VulkanError> allocate();
    std::optional<VulkanError> reset() const; // only when encoder not in use
private:
    explicit VulkanCommandEncoder(std::shared_ptr<VulkanDevice> device, vk::raii::CommandPool commandPool,
        std::vector<vk::raii::CommandBuffer> commandBuffers) : device(std::move(device)),
        commandPool(std::move(commandPool)), commandBuffers(std::move(commandBuffers)) {}
    static constexpr size_t allocationCount = 16;

    std::shared_ptr<VulkanDevice> device;
    vk::raii::CommandPool commandPool;
    std::vector<vk::raii::CommandBuffer> commandBuffers;
};

}