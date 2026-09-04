//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanDevice.h"
#include <ranges>

#include "Device.h"
#include "VulkanInstance.h"
#include "../Error.h"

namespace rhi::vulkan {

vk::DeviceSize VulkanDevice::getDeviceLocalMemory(const vk::raii::PhysicalDevice &physicalDevice) {
    const auto memoryProperties = physicalDevice.getMemoryProperties();
    vk::DeviceSize total = 0;
    for (const auto&[size, flags] : memoryProperties.memoryHeaps) {
        if (flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
            total += size;
        }
    }

    return total;
}

float VulkanDevice::getPhysicalDeviceScore(const vk::raii::PhysicalDevice &physicalDevice,
                                           const DeviceImplCreateInfo<VulkanApi> &createInfo) {
    if (auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            std::ranges::none_of(queueFamilies, [](const auto& queueFamily)
                { return !!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics); })) {
        return 0;
    }
    if (createInfo.surface != nullptr) {
        if (std::ranges::none_of(physicalDevice.enumerateDeviceExtensionProperties(), [](const auto& ext) {
            return strcmp(ext.extensionName, surfaceExtension) == 0;
        })) return 0;
    }

    // --- score ---
    float score = 1000;

    // - device type -
    const bool lowPower = createInfo.powerPreferences == renderium::PowerPreferences::LowPower;
    const float gpuFactor = lowPower ? 0.5f : 2.0f;
    const float cpuFactor = lowPower ? 2.0f : 0.5f;
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

VulkanDevice::PhysicalDeviceResult VulkanDevice::createPhysicalDevice(const VulkanInstance &instance,
                                                            const DeviceImplCreateInfo<VulkanApi>& createInfo) {
    const std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesFound);
    int theChosenIndex = 0; float highestScore = 0;

    for (int i = 0; i < physicalDevices.size(); i++) {
        const float score = getPhysicalDeviceScore(physicalDevices[i], createInfo);
        if (score <= highestScore) continue;
        highestScore = score;
        theChosenIndex = i;
    }
    if (highestScore == 0.0f) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesSuitable);
    return PhysicalDeviceResult::ok(physicalDevices[theChosenIndex]);
}

VulkanDevice::QueueIndexResult VulkanDevice::getQueueIndex(const vk::QueueFlagBits flags, const vk::raii::PhysicalDevice &physicalDevice) {
    const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (flags & queueFamilyProperties[i].queueFlags) {
            return QueueIndexResult::ok(i);
        }
    }
    return QueueIndexResult::err(VulkanError::NoQueueFamilySuitable);
}

VulkanDevice::VulkanDeviceResult VulkanDevice::createDevice(const vk::raii::PhysicalDevice &physicalDevice,
                                                            const DeviceImplCreateInfo<VulkanApi>& createInfo) {
    auto queueIndexResult = getQueueIndex(vk::QueueFlagBits::eGraphics, physicalDevice);
    if (!queueIndexResult.isOk()) return VulkanDeviceResult::err(queueIndexResult.unwrapError());
    uint32_t queueFamilyIndex = queueIndexResult.unwrap();

    vk::StructureChain<vk::PhysicalDeviceFeatures2 ,vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain{
            {.features = {}},
            {.shaderDrawParameters = true},
            {.synchronization2 = true, .dynamicRendering = true},
            {.extendedDynamicState = true}
    };

    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo queueCreateInfo{
        .queueFamilyIndex = queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = &surfaceExtension
    };
    return VulkanDeviceResult::ok(
        {std::move(vk::raii::Device(physicalDevice, deviceCreateInfo)), queueFamilyIndex});
}

VulkanDevice::DeviceResult VulkanDevice::create(const VulkanInstance &instance,
    const DeviceImplCreateInfo<VulkanApi>& createInfo) {
    // physical device
    auto physicalDeviceResult = createPhysicalDevice(instance, createInfo);
    if (!physicalDeviceResult.isOk()) return DeviceResult::err(physicalDeviceResult.unwrapError());
    vk::raii::PhysicalDevice physicalDevice = physicalDeviceResult.unwrap();

    // device
    auto deviceResult = createDevice(physicalDevice, createInfo);
    if (!deviceResult.isOk()) return DeviceResult::err(deviceResult.unwrapError());
    auto [device, queueFamilyIndex] = deviceResult.unwrap();

    return DeviceResult::ok( VulkanDevice(
        std::move(physicalDevice), std::move(device), queueFamilyIndex));
}

}