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
struct DeviceDescriptor;
}

namespace rhi::vulkan {
class VulkanAdapter;
class VulkanInstance;
enum class VulkanError;

class VulkanDevice {
    // --- device ---
    using QueueIndexResult = renderium::Result<uint32_t, VulkanError>;
    static QueueIndexResult getQueueIndex(vk::QueueFlagBits flags, const vk::raii::PhysicalDevice& physicalDevice);
    using VulkanDeviceResult = renderium::Result<std::pair<vk::raii::Device, uint32_t>, VulkanError>;
    static VulkanDeviceResult createDevice(const vk::raii::PhysicalDevice& physicalDevice,
        const renderium::DeviceDescriptor& descriptor, bool surfaceSupport);
public:
    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    static DeviceResult create(const VulkanInstance& instance, const vk::raii::PhysicalDevice& physicalDevice,
        const renderium::DeviceDescriptor& descriptor, bool surfaceSupport);
    [[nodiscard]] const vk::raii::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
    [[nodiscard]] const vk::raii::Device& getDevice() const { return device; }
    [[nodiscard]] uint32_t getQueueFamilyIndex() const { return queueFamilyIndex; }
private:
    VulkanDevice(const vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device device, const uint32_t familyQueueIndex)
        : physicalDevice(physicalDevice), queueFamilyIndex(familyQueueIndex), device(std::move(device)) {}

    const vk::raii::PhysicalDevice& physicalDevice;
    uint32_t queueFamilyIndex;
    vk::raii::Device device;
    friend class VulkanQueue;
};

}
