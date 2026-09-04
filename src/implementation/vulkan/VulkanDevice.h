//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../DeviceImpl.h"
#include "../API.h"

namespace rhi {
struct VulkanApi;
}

namespace renderium {
struct DeviceCreateInfo;
}

namespace rhi::vulkan {
class VulkanInstance;
enum class VulkanError;

class VulkanDevice {
    // --- physical device ---
    static constexpr const char* surfaceExtension = vk::KHRSwapchainExtensionName;
    static vk::DeviceSize getDeviceLocalMemory(const vk::raii::PhysicalDevice& physicalDevice);
    static float getPhysicalDeviceScore(const vk::raii::PhysicalDevice& physicalDevice, const DeviceImplCreateInfo<VulkanApi>& createInfo);
    using PhysicalDeviceResult = renderium::Result<vk::raii::PhysicalDevice, VulkanError>;
    static PhysicalDeviceResult createPhysicalDevice(const VulkanInstance& instance, const DeviceImplCreateInfo<VulkanApi>& createInfo);

    // --- device ---
    using QueueIndexResult = renderium::Result<uint32_t, VulkanError>;
    static QueueIndexResult getQueueIndex(vk::QueueFlagBits flags, const vk::raii::PhysicalDevice& physicalDevice);
    using VulkanDeviceResult = renderium::Result<std::pair<vk::raii::Device, uint32_t>, VulkanError>;
    static VulkanDeviceResult createDevice(const vk::raii::PhysicalDevice& physicalDevice, const DeviceImplCreateInfo<VulkanApi>& createInfo);
public:
    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    static DeviceResult create(const VulkanInstance& instance, const DeviceImplCreateInfo<VulkanApi>& createInfo);
    [[nodiscard]] const vk::raii::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
    [[nodiscard]] const vk::raii::Device& getDevice() const { return device; }
    uint32_t getQueueFamilyIndex() const { return queueFamilyIndex; }
private:
    VulkanDevice(vk::raii::PhysicalDevice physicalDevice, vk::raii::Device device, const uint32_t familyQueueIndex)
        : physicalDevice(std::move(physicalDevice)), queueFamilyIndex(familyQueueIndex), device(std::move(device)) {}

    vk::raii::PhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
    vk::raii::Device device;
    friend class VulkanQueue;
};

}
