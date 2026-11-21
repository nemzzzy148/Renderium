//
// Created by Nemesis Verstraete on 09/11/2026.
//

#include "VulkanCommandBuffer.h"

#include "../Error.h"
#include "VulkanDevice.h"


namespace rhi::vulkan {

VulkanCommandPool::CommandPoolResult VulkanCommandPool::create(std::weak_ptr<VulkanDevice> weakDevice,
    const uint32_t queueFamilyIndex) {
    auto device = weakDevice.lock();
    if (!device) return CommandPoolResult::err(VulkanError::InvalidArgument);

    const vk::CommandPoolCreateInfo poolInfo {
        .queueFamilyIndex = queueFamilyIndex
    };


    auto [poolResult, pool] = device->getDevice().createCommandPool(poolInfo);
    if (poolResult != vk::Result::eSuccess) {
        return CommandPoolResult::err(VulkanError::CommandPoolCreationError);
    }

    const vk::CommandBufferAllocateInfo buffersAllocInfo {
        .commandPool = pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = allocationCount
    };
    auto [buffersResult, cBs] = device->getDevice().allocateCommandBuffers(buffersAllocInfo);
    if (buffersResult != vk::Result::eSuccess) {
        return CommandPoolResult::err(VulkanError::CommandBufferCreationError);
    }

    std::vector<std::shared_ptr<vk::raii::CommandBuffer>> commandBuffers;
    commandBuffers.reserve(allocationCount);
    for (auto& commandBuffer : cBs) {
        commandBuffers.emplace_back(std::make_shared<vk::raii::CommandBuffer>(std::move(commandBuffer)));
    }


    return CommandPoolResult::ok(std::make_unique<VulkanCommandPool>(
        VulkanCommandPool(std::move(weakDevice), std::move(pool),
            std::move(commandBuffers))));
}

std::optional<VulkanError> VulkanCommandPool::allocate() {
    const auto device = weakDevice.lock();
    if (!device) return VulkanError::UnhandledException;

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
    // const uint32_t oldSize = commandBuffers.size();
    for (auto& commandBuffer: cBs) {
        commandBuffers.push_back(std::make_unique<vk::raii::CommandBuffer>(std::move(commandBuffer)));
    }

    return std::nullopt; // no error
}

std::optional<VulkanError> VulkanCommandPool::reset() const {
    const auto device = weakDevice.lock();
    if (!device) return VulkanError::UnhandledException;

    const auto result = commandPool.reset();
    if (result != vk::Result::eSuccess) {
        return VulkanError::FailedToResetCommandPool;
    }
    return std::nullopt;
}

void VulkanCommandBuffer::begin() {
}
void VulkanCommandBuffer::end() {
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    if (const auto device = weakDevice.lock()) {
        device->releaseCommandBuffer(handle);
    }
}

}