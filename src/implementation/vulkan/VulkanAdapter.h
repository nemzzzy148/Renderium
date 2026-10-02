//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once
#include <memory>
#include <utility>

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
    explicit VulkanAdapter(std::shared_ptr<VulkanInstance> instance, std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice,
        const bool surfaceSupport) : instance(std::move(instance)), physicalDevice(std::move(physicalDevice)),
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
    static AdapterResult create(std::shared_ptr<VulkanInstance> instance, const AdapterOptions& options);

    // --- device ---
    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    [[nodiscard]] DeviceResult requestDevice(const renderium::DeviceDescriptor& descriptor) const;
private:
    std::shared_ptr<VulkanInstance> instance;
    std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice;
    bool surfaceSupport = false;
};

}
