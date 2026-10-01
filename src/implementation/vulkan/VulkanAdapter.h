//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once
#include "../AdapterImpl.h"
#include "vulkan/vulkan_raii.hpp"

namespace rhi {
struct VulkanApi;
}

namespace renderium {
struct DeviceDescriptor;
}

namespace rhi::vulkan {
class VulkanDevice;
enum class VulkanError;
class VulkanInstance;

class VulkanAdapter {
public:
    explicit VulkanAdapter(const VulkanInstance& instance, vk::raii::PhysicalDevice physicalDevice,
        const bool surfaceSupport) : instance(instance), physicalDevice(std::move(physicalDevice)),
        surfaceSupport(surfaceSupport) {}
    using AdapterOptions = AdapterImplOptions<VulkanApi>;

    // --- physical device ---
    static constexpr const char* surfaceExtension = vk::KHRSwapchainExtensionName;
    static vk::DeviceSize getDeviceLocalMemory(const vk::raii::PhysicalDevice& physicalDevice);
    static float powerPreferenceFactor(renderium::PowerPreference powerPreference, bool gpu);
    static float getPhysicalDeviceScore(const vk::raii::PhysicalDevice& physicalDevice, const AdapterOptions& options);
    using PhysicalDeviceResult = renderium::Result<vk::raii::PhysicalDevice, VulkanError>;
    static PhysicalDeviceResult createPhysicalDevice(const VulkanInstance& instance, const AdapterOptions& options);

    using AdapterResult = renderium::Result<VulkanAdapter, VulkanError>;
    static AdapterResult create(const VulkanInstance& instance, const AdapterOptions& options);

    // --- device ---
    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    DeviceResult requestDevice(const renderium::DeviceDescriptor& descriptor) const;
private:
    const VulkanInstance& instance;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    bool surfaceSupport = false;
};

}
