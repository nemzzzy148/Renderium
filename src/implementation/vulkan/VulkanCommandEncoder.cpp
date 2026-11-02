//
// Created by Nemesis Verstraete on 02/11/2026.
//

#include "VulkanCommandEncoder.h"

#include "../Error.h"
#include "VulkanDevice.h"

namespace rhi::vulkan {

VulkanCommandEncoder::CommandEncoderResult VulkanCommandEncoder::create(std::shared_ptr<VulkanDevice> device,
        const uint32_t queueFamilyIndex) {
    const vk::CommandPoolCreateInfo poolInfo {
        .queueFamilyIndex = queueFamilyIndex
    };
    auto [poolResult, pool] = device->getDevice().createCommandPool(poolInfo);
    if (poolResult != vk::Result::eSuccess) {
        return CommandEncoderResult::err(VulkanError::CommandPoolCreationError);
    }

    const vk::CommandBufferAllocateInfo buffersAllocInfo {
        .commandPool = pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = allocationCount
    };
    auto [buffersResult, commandBuffers] = device->getDevice().allocateCommandBuffers(buffersAllocInfo);
    if (buffersResult != vk::Result::eSuccess) {
        return CommandEncoderResult::err(VulkanError::CommandBufferCreationError);
    }

    return CommandEncoderResult::ok(std::make_shared<VulkanCommandEncoder>(
        VulkanCommandEncoder(std::move(device), std::move(pool), std::move(commandBuffers))));
}

std::optional<VulkanError> VulkanCommandEncoder::allocate() {
    const vk::CommandBufferAllocateInfo buffersAllocInfo {
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = allocationCount
    };
    auto [buffersResult, cBs] = device->getDevice().allocateCommandBuffers(buffersAllocInfo);
    if (buffersResult != vk::Result::eSuccess) {
        return VulkanError::CommandBufferCreationError;
    }

    commandBuffers.reserve(commandBuffers.size() + allocationCount);
    for (auto& commandBuffer: cBs) {
        commandBuffers.push_back(std::move(commandBuffer));
    }

    return std::nullopt; // no error
}

std::optional<VulkanError> VulkanCommandEncoder::reset() const {
    const auto result = commandPool.reset();
    if (result != vk::Result::eSuccess) {
        return VulkanError::FailedToResetCommandPool;
    }
    return std::nullopt;
}

}