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

VulkanDevice::DeviceResult VulkanDevice::create(std::shared_ptr<VulkanInstance> instance,
        std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice,
        const renderium::DeviceDescriptor& descriptor,
        const bool surfaceSupport) {
    // device
    auto deviceResult = createDevice(*physicalDevice, descriptor, surfaceSupport);
    if (!deviceResult.isOk()) return DeviceResult::err(deviceResult.unwrapError());
    auto [device, queueFamilyIndex] = deviceResult.unwrap();

    return DeviceResult::ok(VulkanDevice(
        std::move(instance), std::move(physicalDevice), std::move(device), queueFamilyIndex));
}

VulkanDevice::MemoryManager::MemoryManager(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device)
    : physicalDevice(physicalDevice), device(device) {}

MemoryAllocation VulkanDevice::MemoryManager::bufferAllocation(const uint32_t alignment, const uint64_t size,
    const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryProperties) {
    if (size == 0) return {};

    const auto memoryType = findMemoryType(typeFilter, memoryProperties);
    if (!memoryType) return{}; // no available memory type found

    return allocate(bufferHeaps[*memoryType], alignment, size, *memoryType);
}

MemoryAllocation VulkanDevice::MemoryManager::textureAllocation(const uint32_t alignment, const uint64_t size,
    const uint32_t typeFilter, const vk::MemoryPropertyFlags memoryProperties) {
    if (size == 0) return {};

    const auto memoryType = findMemoryType(typeFilter, memoryProperties);
    if (!memoryType) return{}; // no available memory type found

    return allocate(textureHeaps[*memoryType], alignment, size, *memoryType);
}

void VulkanDevice::MemoryManager::freeBuffer(const int32_t poolIndex, const uint32_t nodeIndex,
        const uint32_t memoryType) {
    if (!bufferHeaps.contains(memoryType)) return; // no allocation made with that memory type

    // free the allocation in the system so another allocation can be made on the same memory
    bufferHeaps[memoryType].tlsf.free(poolIndex, nodeIndex);
}

void VulkanDevice::MemoryManager::freeTexture(const uint32_t poolIndex, const uint32_t nodeIndex,
        const uint32_t memoryType) {
    if (!textureHeaps.contains(memoryType)) return; // no allocation made with that memory type

    // free the allocation in the system so another allocation can be made on the same memory
    textureHeaps[memoryType].tlsf.free(poolIndex, nodeIndex);
}

std::optional<uint32_t> VulkanDevice::MemoryManager::findMemoryType(const uint32_t typeFilter,
    const vk::MemoryPropertyFlags properties) const {
    const vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    return std::nullopt; // no compatible memory types found
}

VulkanDevice::MemoryManager::DeviceMemoryResult VulkanDevice::MemoryManager::allocateDeviceMemory(const uint64_t size,
        const uint32_t memoryType) const {
    const vk::MemoryAllocateInfo allocInfo {
        .allocationSize = size,
        .memoryTypeIndex = memoryType
    };

    vk::DeviceMemory memory{};
    const auto result = (*device).allocateMemory(&allocInfo, nullptr, &memory);

    if (result == vk::Result::eErrorOutOfDeviceMemory) {
        return DeviceMemoryResult::err(VulkanError::RanOutOfDeviceMemory);
    }
    if (result == vk::Result::eErrorOutOfHostMemory) {
        return DeviceMemoryResult::err(VulkanError::RanOutOfHostMemory);
    }

    return DeviceMemoryResult::ok(vk::raii::DeviceMemory(device, memory));
}

MemoryAllocation VulkanDevice::MemoryManager::allocate(Heap &heap, const uint32_t alignment, uint64_t size,
    const uint32_t memoryType) const {
    if (size == 0) return {};

    auto&[memory, tlsf] = heap;

    // the object could be just instantiated
    const TLSFAllocation allocation = tlsf.allocate(alignment, size);

    if (!allocation.valid) {
        size = std::max(size, DEFAULT_SIZE);

        // device memory allocation
        auto allocationResult = allocateDeviceMemory(size, memoryType);

        if (!allocationResult.isOk()) {
            return {}; // allocation failed (no vram left, ...)
        }

        const uint32_t poolIdx = tlsf.createPool({size});
        const TLSFAllocation fitAllocation = tlsf.allocate(poolIdx, alignment, size);
        if (!fitAllocation.valid) return {}; // this would normally not happen

        memory.insert_or_assign(
            {poolIdx, fitAllocation.nodeIndex}, std::move(allocationResult.unwrap()));

        return {poolIdx, fitAllocation.nodeIndex, true};
    }

    return {allocation.poolIndex, allocation.nodeIndex, true};
}

}
