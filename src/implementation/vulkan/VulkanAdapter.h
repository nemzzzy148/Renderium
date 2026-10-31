//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once
#include <memory>
#include <utility>

#include "VulkanInstance.h"
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
        const bool surfaceSupport, const uint32_t queueFamilyIndex) : instance(std::move(instance)),
        physicalDevice(std::move(physicalDevice)), queueFamilyIndex(queueFamilyIndex), surfaceSupport(surfaceSupport) {}
    using AdapterOptions = AdapterImplOptions<VulkanApi>;

    // --- physical device ---
    static vk::DeviceSize getDeviceLocalMemory(const vk::raii::PhysicalDevice& physicalDevice);
    static float powerPreferenceFactor(renderium::PowerPreference powerPreference, bool gpu);
    static float getPhysicalDeviceScore(const vk::raii::PhysicalDevice& physicalDevice, const AdapterOptions& options);
    using PhysicalDeviceResult = renderium::Result<vk::raii::PhysicalDevice, VulkanError>;
    static PhysicalDeviceResult createPhysicalDevice(const VulkanInstance& instance, const AdapterOptions& options);

    using QueueIndexResult = renderium::Result<uint32_t, VulkanError>;
    static QueueIndexResult selectQueueFamilyIndex(vk::QueueFlagBits flags,
        const vk::raii::PhysicalDevice& physicalDevice);

    using AdapterResult = renderium::Result<VulkanAdapter, VulkanError>;
    static AdapterResult create(std::shared_ptr<VulkanInstance> instance, const AdapterOptions& options);

private:
    struct FormatRequirement {
        vk::Format format;
        vk::FormatFeatureFlags features;
    };

    [[nodiscard]] uint32_t getVulkanVersion() const;
    [[nodiscard]] bool hasExtension(const char* extensionName) const;
    [[nodiscard]] bool hasFormatFeature(vk::Format format, vk::FormatFeatureFlags neededFlags) const;
    [[nodiscard]] bool hasFormatFeature(FormatRequirement requirement) const;
    bool hasAllFormats(const std::vector<vk::Format>& formats, vk::FormatFeatureFlags neededFlags) const;
    bool image3DSupported(vk::Format format) const;
    bool hasAllFormatRequirements(const std::vector<FormatRequirement>& requirements);
public:

    [[nodiscard]] renderium::SupportedFeatures requestFeatures() const;
    [[nodiscard]] renderium::SupportedLimits requestLimits() const;
    [[nodiscard]] renderium::AdapterInfo requestAdapterInfo() const;

    // --- device ---
    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    [[nodiscard]] DeviceResult requestDevice(const renderium::DeviceDescriptor& descriptor) const;
private:
    std::shared_ptr<VulkanInstance> instance;
    std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice;

    uint32_t queueFamilyIndex;

    bool surfaceSupport = false;
};

}
