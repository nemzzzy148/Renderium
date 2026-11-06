//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <Result.h>

namespace rhi::vulkan {
enum class VulkanError;
class VulkanDevice;

class VulkanCommandPool {
public:
    using CommandPoolResult = renderium::Result<std::shared_ptr<VulkanCommandPool>, VulkanError>;
    static CommandPoolResult create(std::shared_ptr<VulkanDevice> device, uint32_t queueFamilyIndex);

    [[nodiscard]] size_t size() const { return commandBuffers.size(); }

    std::optional<VulkanError> allocate();
    std::optional<VulkanError> reset() const; // Only when command buffers are not in use! Multithreading

    using CommandBufferResult = renderium::Result<std::shared_ptr<vk::raii::CommandBuffer>, VulkanError>;
    CommandBufferResult getAvailableCommandBuffer();
private:
    explicit VulkanCommandPool(std::weak_ptr<VulkanDevice> device, vk::raii::CommandPool commandPool,
    std::vector<std::shared_ptr<vk::raii::CommandBuffer>> commandBuffers, std::vector<uint32_t> freeCommandBuffers)
        : weakDevice(std::move(device)), commandPool(std::move(commandPool)),
        commandBuffers(std::move(commandBuffers)), freeCommandBuffers(std::move(freeCommandBuffers)) {}

    static constexpr size_t allocationCount = 16;

    static void addFree(std::vector<uint32_t>& free, uint32_t count, uint32_t begin);

    std::weak_ptr<VulkanDevice> weakDevice;
    vk::raii::CommandPool commandPool;
    std::vector<std::shared_ptr<vk::raii::CommandBuffer>> commandBuffers;
    std::vector<uint32_t> freeCommandBuffers;
};

class VulkanCommandEncoder {
public:
    using CommandEncoderResult = renderium::Result<std::unique_ptr<VulkanCommandEncoder>, VulkanError>;
    static CommandEncoderResult create(std::weak_ptr<VulkanDevice> device, std::shared_ptr<VulkanCommandPool> commandPool);

    VulkanCommandPool::CommandBufferResult getAvailableCommandBuffer() const;

    ~VulkanCommandEncoder();
private:
    explicit VulkanCommandEncoder(std::weak_ptr<VulkanDevice> device, std::shared_ptr<VulkanCommandPool> commandPool)
        : weakDevice(std::move(device)), commandPool(std::move(commandPool)) {}

    std::weak_ptr<VulkanDevice> weakDevice;
    std::shared_ptr<VulkanCommandPool> commandPool;

    friend class VulkanDevice;
};

}