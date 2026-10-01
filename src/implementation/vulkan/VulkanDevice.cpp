//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanDevice.h"
#include <ranges>

#include "Device.h"
#include "VulkanAdapter.h"
#include "VulkanInstance.h"
#include "../Error.h"

namespace rhi::vulkan {

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
    const renderium::DeviceDescriptor& descriptor, const bool surfaceSupport) {
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
        .enabledExtensionCount = static_cast<uint32_t>(surfaceSupport ? 1 : 0),
        .ppEnabledExtensionNames = &VulkanAdapter::surfaceExtension
    };
    return VulkanDeviceResult::ok(
        {std::move(vk::raii::Device(physicalDevice, deviceCreateInfo)), queueFamilyIndex});
}

VulkanDevice::DeviceResult VulkanDevice::create(const VulkanInstance& instance,
        const vk::raii::PhysicalDevice& physicalDevice, const renderium::DeviceDescriptor& descriptor,
        const bool surfaceSupport) {
    // device
    auto deviceResult = createDevice(physicalDevice, descriptor, surfaceSupport);
    if (!deviceResult.isOk()) return DeviceResult::err(deviceResult.unwrapError());
    auto [device, queueFamilyIndex] = deviceResult.unwrap();

    return DeviceResult::ok( VulkanDevice(
        physicalDevice, std::move(device), queueFamilyIndex));
}

}