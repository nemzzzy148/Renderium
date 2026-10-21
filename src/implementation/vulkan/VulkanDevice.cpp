//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanDevice.h"
#include <ranges>

#include "Device.h"
#include "Texture.h"
#include "Buffer.h"
#include "VulkanAdapter.h"
#include "VulkanConversion.h"
#include "VulkanInstance.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
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

VulkanDevice::VulkanDevice(VulkanDevice&& other)
    : std::enable_shared_from_this<VulkanDevice>(), instance(std::move(other.instance)),
      physicalDevice(std::move(other.physicalDevice)), queueFamilyIndex(other.queueFamilyIndex),
      device(std::move(other.device)), memoryManager(std::move(other.memoryManager),
          *physicalDevice, *device) {}

VulkanDevice::DeviceResult VulkanDevice::create(std::shared_ptr<VulkanInstance> instance,
        std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice,
        const renderium::DeviceDescriptor& descriptor,
        const bool surfaceSupport) {
    // device
    auto deviceResult = createDevice(*physicalDevice, descriptor, surfaceSupport);
    if (!deviceResult.isOk()) return DeviceResult::err(deviceResult.unwrapError());
    auto [device, queueFamilyIndex] = deviceResult.unwrap();

    return DeviceResult::ok(VulkanDevice(
        std::move(instance), std::move(physicalDevice),
        std::make_unique<vk::raii::Device>(std::move(device)), queueFamilyIndex));
}

// --- texture and buffer creation ---

VulkanDevice::BufferResult VulkanDevice::createBuffer(const renderium::BufferDescriptor &descriptor) {
    if (descriptor.size == 0) return BufferResult::err(VulkanError::InvalidBufferSize);

    const vk::BufferUsageFlags usage = VulkanConversion::mapBufferUsages(descriptor.usages);
    const vk::BufferCreateInfo bufferInfo {
        .size = descriptor.size,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive
    };
    auto buffer = vk::raii::Buffer(*device, bufferInfo);

    auto [size, alignment, memoryTypeBits] = buffer.getMemoryRequirements();

    // a better optimization would be map write (visible and coherent) and map read(visible and cached)
    const vk::MemoryPropertyFlags memoryProperties =
        (utils::any(descriptor.usages & (renderium::BufferUsages::MapRead | renderium::BufferUsages::MapWrite))) ?
        vk::MemoryPropertyFlagBits::eHostVisible & vk::MemoryPropertyFlagBits::eHostCoherent // true
        : vk::MemoryPropertyFlagBits::eDeviceLocal; // false


    // device memory for the image
    const auto allocation = memoryManager.bufferAllocation(alignment, size, memoryTypeBits, memoryProperties);
    if (!allocation.valid) {
        BufferResult::err(allocation.error);
    }

    return BufferResult::ok(VulkanBuffer(shared_from_this(),
        [this](const uint32_t poolIdx, const uint32_t nodeIdx, const uint32_t memoryType) {
        memoryManager.freeBuffer(poolIdx, nodeIdx, memoryType);
    }, allocation));
}

VulkanDevice::TextureResult VulkanDevice::createTexture(const renderium::TextureDescriptor &descriptor) {
    if (descriptor.size.height == 0 || descriptor.size.width == 0)
        return TextureResult::err(VulkanError::InvalidImageDimensions);

    const vk::ImageCreateInfo imageInfo {
        .imageType = VulkanConversion::mapTextureDimension(descriptor.dimension),
        .format = VulkanConversion::mapTextureFormat(descriptor.format),
        .extent = VulkanConversion::mapExtent3D(descriptor.size),
        .mipLevels = descriptor.mipLevelCount,
        .arrayLayers = 1,
        .samples = VulkanConversion::mapSampleCount(descriptor.sampleCount),
        .tiling = vk::ImageTiling::eOptimal,
        .usage = VulkanConversion::mapTextureUsages(descriptor.usage),
        .sharingMode = vk::SharingMode::eExclusive
    };
    auto image = vk::raii::Image(*device, imageInfo);

    auto [size, alignment, memoryTypeBits] = image.getMemoryRequirements();

    // device memory for the image
    const auto allocation = memoryManager.textureAllocation(alignment, size,
        memoryTypeBits, VulkanTexture::memoryProperties);
    if (!allocation.valid) {
        TextureResult::err(allocation.error);
    }

    return TextureResult::ok(VulkanTexture(shared_from_this(),
    [this](const uint32_t poolIdx, const uint32_t nodeIdx, const uint32_t memoryType) {
        memoryManager.freeTexture(poolIdx, nodeIdx, memoryType); // destructor callback
    }, allocation));
}

VulkanDevice::MemoryManager::MemoryManager(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device)
    : physicalDevice(physicalDevice), device(device) {}

VulkanDevice::MemoryManager::MemoryManager(MemoryManager&& other,
    const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device)
    : physicalDevice(physicalDevice), device(device), bufferHeaps(std::move(other.bufferHeaps)),
      textureHeaps(std::move(other.textureHeaps)) {}

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

void VulkanDevice::MemoryManager::freeBuffer(const uint32_t poolIndex, const uint32_t nodeIndex,
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
        return DeviceMemoryResult::err(VulkanError::OutOfDeviceMemory);
    }
    if (result == vk::Result::eErrorOutOfHostMemory) {
        return DeviceMemoryResult::err(VulkanError::OutOfHostMemory);
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
            return MemoryAllocation(allocationResult.unwrapError());
        }

        const uint32_t poolIdx = tlsf.createPool({size});
        const TLSFAllocation fitAllocation = tlsf.allocate(poolIdx, alignment, size);
        if (!fitAllocation.valid) return {}; // this would normally not happen

        memory.insert_or_assign(
            {poolIdx, fitAllocation.nodeIndex}, std::move(allocationResult.unwrap()));

        return {poolIdx, fitAllocation.nodeIndex, memoryType, true};
    }

    return {allocation.poolIndex, allocation.nodeIndex, memoryType, true};
}

}