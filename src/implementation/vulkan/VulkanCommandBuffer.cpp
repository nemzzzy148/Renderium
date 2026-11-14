//
// Created by Nemesis Verstraete on 09/11/2026.
//

#include "VulkanCommandBuffer.h"

#include <numeric>

#include "../Error.h"
#include "VulkanDevice.h"


namespace rhi::vulkan {

VulkanCommandPool::CommandPoolResult VulkanCommandPool::create(std::shared_ptr<VulkanDevice> device,
    const uint32_t queueFamilyIndex) {
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
        commandBuffers.emplace_back(std::make_unique<vk::raii::CommandBuffer>(std::move(commandBuffer)));
    }

    std::vector<uint32_t> free;
    addFree(free, allocationCount, 0);


    return CommandPoolResult::ok(std::make_shared<VulkanCommandPool>(
        VulkanCommandPool(std::move(device), std::move(pool),
            std::move(commandBuffers), std::move(free))));
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
    const uint32_t oldSize = commandBuffers.size();
    for (auto& commandBuffer: cBs) {
        commandBuffers.push_back(std::make_unique<vk::raii::CommandBuffer>(std::move(commandBuffer)));
    }

    addFree(freeCommandBuffers, allocationCount, oldSize);

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

VulkanCommandPool::CommandBufferResult VulkanCommandPool::getAvailableCommandBuffer() {
    if (freeCommandBuffers.empty()) {
        if (const auto allocationResult = allocate()) {
            return CommandBufferResult::err(*allocationResult);
        }
    }
    const uint32_t index = freeCommandBuffers.back();
    freeCommandBuffers.pop_back();

    return CommandBufferResult::ok(commandBuffers[index]);
}

void VulkanCommandPool::addFree(std::vector<uint32_t> &free, const uint32_t count, const uint32_t begin) {
    const size_t oldSize = free.size();
    free.reserve(oldSize + count);
    std::iota(free.begin() + oldSize, free.end(), begin);
}

VulkanCommandBuffer::CommandBufferResult VulkanCommandBuffer::create(std::weak_ptr<VulkanDevice> device,
    std::shared_ptr<VulkanCommandPool> commandPool) {
    return CommandBufferResult::ok(
        std::unique_ptr<VulkanCommandBuffer>(new VulkanCommandBuffer(std::move(device),
            std::move(commandPool))));
}

VulkanCommandPool::CommandBufferResult VulkanCommandBuffer::getAvailableCommandBuffer() const {
    return commandPool->getAvailableCommandBuffer();
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    if (const auto device = weakDevice.lock()) {
        device->releaseCommandEncoder(&*commandPool);
    }
}

}