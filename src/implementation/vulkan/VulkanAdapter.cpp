//
// Created by Nemesis Verstraete on 30/09/2026.
//

#include "VulkanAdapter.h"

#include "VulkanInstance.h"
#include "../Error.h"

namespace rhi::vulkan {

vk::DeviceSize VulkanAdapter::getDeviceLocalMemory(const vk::raii::PhysicalDevice &physicalDevice) {
    const auto memoryProperties = physicalDevice.getMemoryProperties();
    vk::DeviceSize total = 0;
    for (const auto&[size, flags] : memoryProperties.memoryHeaps) {
        if (flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
            total += size;
        }
    }

    return total;
}

float VulkanAdapter::powerPreferenceFactor(const renderium::PowerPreference powerPreference, const bool gpu) {
    switch (powerPreference) {
        case renderium::PowerPreference::None: return 1.0f;
        case renderium::PowerPreference::HighPerformance: return gpu ? 2.0f : 0.5f;
        case renderium::PowerPreference::LowPower: return gpu ? 0.5f : 2.0f;
        default: return 1.0f;
    }
}

float VulkanAdapter::getPhysicalDeviceScore(const vk::raii::PhysicalDevice& physicalDevice,
                                            const AdapterOptions& options) {
    if (auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            std::ranges::none_of(queueFamilies, [](const auto& queueFamily)
                { return !!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics); })) {
        return 0;
                }
    if (options.surface != nullptr) {
        if (std::ranges::none_of(physicalDevice.enumerateDeviceExtensionProperties(), [](const auto& ext) {
            return strcmp(ext.extensionName, surfaceExtension) == 0;
        })) return 0;
    }

    // --- score ---
    float score = 1000;

    // - device type -
    const float gpuFactor = powerPreferenceFactor(options.powerPreference, true);
    const float cpuFactor = powerPreferenceFactor(options.powerPreference, false);
    switch (const vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
            physicalDeviceProperties.deviceType) {
                case vk::PhysicalDeviceType::eIntegratedGpu: score *= cpuFactor; break;
                case vk::PhysicalDeviceType::eDiscreteGpu: score *= gpuFactor; break;
                case vk::PhysicalDeviceType::eCpu: score *= cpuFactor; break;
                default: return 0;
            }

    // - video ram -
    const float vRam = static_cast<float>(getDeviceLocalMemory(physicalDevice)) / (1024.f * 1024.f * 1024.f);
    score+=std::log2f(vRam);

    return score;
}

VulkanAdapter::PhysicalDeviceResult VulkanAdapter::createPhysicalDevice(const VulkanInstance& instance,
                                                                        const AdapterOptions& options) {
    const std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesFound);
    int theChosenIndex = 0; float highestScore = 0;

    for (int i = 0; i < physicalDevices.size(); i++) {
        const float score = getPhysicalDeviceScore(physicalDevices[i], options);
        if (score <= highestScore) continue;
        highestScore = score;
        theChosenIndex = i;
    }
    if (highestScore == 0.0f) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesSuitable);
    return PhysicalDeviceResult::ok(physicalDevices[theChosenIndex]);
}

VulkanAdapter::AdapterResult VulkanAdapter::create(const VulkanInstance& instance, const AdapterOptions& options) {
    auto physicalDeviceResult = createPhysicalDevice(instance, options);
    if (!physicalDeviceResult.isOk()) return AdapterResult::err(physicalDeviceResult.unwrapError());
    return AdapterResult::ok(VulkanAdapter(
        instance, std::move(physicalDeviceResult.unwrap()), options.surface));
}

VulkanAdapter::DeviceResult VulkanAdapter::requestDevice(const renderium::DeviceDescriptor& descriptor) const {
    return VulkanDevice::create(instance, physicalDevice, descriptor, surfaceSupport);
}

}
