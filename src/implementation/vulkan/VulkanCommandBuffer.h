//
// Created by Nemesis Verstraete on 09/11/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"

namespace rhi::vulkan {
class VulkanDevice;
enum class VulkanError;

class VulkanCommandPool {
public:
    using CommandPoolResult = renderium::Result<std::unique_ptr<VulkanCommandPool>, VulkanError>;
    static CommandPoolResult create(std::weak_ptr<VulkanDevice> weakDevice, uint32_t queueFamilyIndex);

    [[nodiscard]] const std::vector<std::shared_ptr<vk::raii::CommandBuffer>>& getCommandBuffers() {
        return commandBuffers;
    }

    std::optional<VulkanError> allocate();
    [[nodiscard]] std::optional<VulkanError> reset() const; // Only when command buffers are not in use! Multithreading

    const std::shared_ptr<vk::raii::CommandBuffer>& operator[] (const size_t index) const {
        return commandBuffers[index];
    }

    [[nodiscard]] size_t size() const { return commandBuffers.size(); }
private:
    explicit VulkanCommandPool(std::weak_ptr<VulkanDevice> device, vk::raii::CommandPool commandPool,
    std::vector<std::shared_ptr<vk::raii::CommandBuffer>> commandBuffers)
        : weakDevice(std::move(device)), commandPool(std::move(commandPool)),
        commandBuffers(std::move(commandBuffers)) {}

    static constexpr size_t allocationCount = 16;

    std::weak_ptr<VulkanDevice> weakDevice;
    vk::raii::CommandPool commandPool;
    std::vector<std::shared_ptr<vk::raii::CommandBuffer>> commandBuffers;
};

class VulkanCommandBuffer {
public:
    void begin();

    void end();

    ~VulkanCommandBuffer();
    explicit VulkanCommandBuffer(std::weak_ptr<VulkanDevice> weakDevice, const uint32_t handle,
        std::shared_ptr<vk::raii::CommandBuffer> commandBuffer) : weakDevice(std::move(weakDevice)),
        handle(handle), commandBuffer(std::move(commandBuffer)) {}

    [[nodiscard]] vk::raii::CommandBuffer& getCommandBuffer() const { return *commandBuffer; }
private:
    std::weak_ptr<VulkanDevice> weakDevice;
    uint32_t handle;
    std::shared_ptr<vk::raii::CommandBuffer> commandBuffer;

    friend class VulkanDevice;

};

}
