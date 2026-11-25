//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanDevice.h"
#include <memory>
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
#include "VulkanCommandBuffer.h"

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
    const bool surfaceSupport, uint32_t queueFamilyIndex) {

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

    std::vector<const char*> extensions;
    if (surfaceSupport) {
        extensions.push_back(vk::KHRSwapchainExtensionName);
    }

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    auto [result, device] = physicalDevice.createDevice(deviceCreateInfo);
    if (result != vk::Result::eSuccess) {
        return VulkanDeviceResult::err(VulkanError::DeviceCreationFailed);
    }
    return VulkanDeviceResult::ok(std::move(device));
}

VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
    : instance(std::move(other.instance)), physicalDevice(std::move(other.physicalDevice)),
      queueFamilyIndex(other.queueFamilyIndex),
      device(std::move(other.device)), memoryManager(std::move(other.memoryManager),
      *physicalDevice, *device), commandPoolManager(std::move(other.commandPoolManager), *this) {}

VulkanDevice::DeviceResult VulkanDevice::create(std::shared_ptr<VulkanInstance> instance,
        std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice,
        const renderium::DeviceDescriptor& descriptor,
        const bool surfaceSupport, const uint32_t queueFamilyIndex) {
    // device
    auto deviceResult = createDevice(*physicalDevice, surfaceSupport, queueFamilyIndex);
    if (!deviceResult.isOk()) {
        return DeviceResult::err(deviceResult.unwrapError());
    }
    auto device = deviceResult.unwrap();

    auto timelineResult = VulkanTimelineSemaphore::create(device);
    if (!timelineResult.isOk()) {
        return DeviceResult::err(timelineResult.unwrapError());
    }


    return DeviceResult::ok(VulkanDevice(
        std::move(instance), std::move(physicalDevice),
        std::make_unique<vk::raii::Device>(std::move(device)),
        std::move(timelineResult.unwrap()), queueFamilyIndex));
}

VulkanDevice::PollResult VulkanDevice::poll() const {
    auto pollingResult = timeline->getValue();
    if (!pollingResult.isOk()) {
        return PollResult::err(pollingResult.unwrapError());
    }
    return PollResult::ok(static_cast<uint32_t>(pollingResult.unwrap()));
}

// --- texture and buffer creation ---

VulkanDevice::BufferResult VulkanDevice::createBuffer(const renderium::BufferDescriptor &descriptor) {
    if (descriptor.size == 0) return BufferResult::err(VulkanError::InvalidBufferSize);

    const vk::BufferUsageFlags usage = VulkanConversion::mapBufferUsages(descriptor.usage);
    const vk::BufferCreateInfo bufferInfo {
        .size = descriptor.size,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive
    };

    auto [result, buffer] = device->createBuffer(bufferInfo);
    if (result != vk::Result::eSuccess) {
        return BufferResult::err(VulkanError::BufferCreationFailed);
    }

    auto [size, alignment, memoryTypeBits] = buffer.getMemoryRequirements();

    // a better optimization would be mapped write(visible and coherent) and map read(visible and cached)
    const vk::MemoryPropertyFlags memoryProperties =
        (utils::any(descriptor.usage & (renderium::BufferUsages::MapRead | renderium::BufferUsages::MapWrite))) ?
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

    auto [result, image] = device->createImage(imageInfo);
    if (result != vk::Result::eSuccess) {
        return TextureResult::err(VulkanError::ImageCreationFailed);
    }

    auto [size, alignment, memoryTypeBits] = image.getMemoryRequirements();

    // device memory for the image
    const auto allocation = memoryManager.textureAllocation(alignment, size,
        memoryTypeBits, VulkanTexture::memoryProperties);
    if (!allocation.valid) {
        return TextureResult::err(allocation.error);
    }

    return TextureResult::ok(VulkanTexture(shared_from_this(),
    [this](const uint32_t poolIdx, const uint32_t nodeIdx, const uint32_t memoryType) {
        memoryManager.freeTexture(poolIdx, nodeIdx, memoryType); // destructor callback
    }, allocation, std::move(image)));
}

VulkanDevice::VulkanTimelineSemaphore::TimelineResult VulkanDevice::VulkanTimelineSemaphore::create(vk::raii::Device& device) {
    constexpr vk::SemaphoreTypeCreateInfo timelineCreateInfo {
        .semaphoreType = vk::SemaphoreType::eTimeline,
        .initialValue = 0
    };
    const vk::SemaphoreCreateInfo semaphoreInfo { .pNext = &timelineCreateInfo };

    auto [result, semaphore] = device.createSemaphore(semaphoreInfo);
    if (result != vk::Result::eSuccess) {
        return TimelineResult::err(VulkanError::TimelineSemaphoreCreationFailed);
    }
    return TimelineResult::ok(std::unique_ptr<VulkanTimelineSemaphore>(
        new VulkanTimelineSemaphore(std::move(semaphore))));
}

VulkanDevice::VulkanTimelineSemaphore::ValueResult VulkanDevice::VulkanTimelineSemaphore::getValue() {
    auto [result, value] = semaphore.getCounterValue();
    if (result != vk::Result::eSuccess) {
        return ValueResult::err(VulkanError::FailedToGetValueOfTimelineSemaphore);
    }
    return ValueResult::ok(value);
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

VulkanDevice::CommandBufferResult VulkanDevice::createCommandBuffer() {
    return commandPoolManager.obtainCommandBuffer();
}

void VulkanDevice::releaseCommandBuffer(const uint32_t handle) {
    commandPoolManager.releaseCommandBuffer(handle);
}

VulkanDevice::CommandPoolManager::CommandPoolManager(VulkanDevice &device) :
    commandPools(std::vector<CommandPoolEntry>()),
    device(device), lock(std::mutex()) {}

VulkanDevice::CommandPoolManager::CommandPoolManager(CommandPoolManager &&commandPoolManager,
                                                     VulkanDevice& device) : commandPools(std::move(commandPoolManager.commandPools)),
                                                                             device(device), lock(std::mutex()) {}

VulkanDevice::CommandPoolManager::CommandBufferResult VulkanDevice::CommandPoolManager::obtainCommandBuffer() {
    auto getTimelineResult = device.timeline->getValue();
    if (!getTimelineResult.isOk()) {
        return CommandBufferResult::err(VulkanError::FailedToGetValueOfTimelineSemaphore);
    }
    uint64_t currentTimelineIndex = getTimelineResult.unwrap();

    std::lock_guard guard(lock);
    auto it = std::ranges::find_if(commandPools,
        [&currentTimelineIndex](auto& pool) {
        if (pool.beingUsed) {
            return false;
        }

        // Pool still has unused command buffers left
        if (pool.lastIdx < pool.commandPool->size()) {
            return true;
        }

        // Pool is full -> check if the GPU is done with all buffers in it
        if (pool.lastSubmittedTimelineIndex <= currentTimelineIndex) {
            auto resetResult = pool.reset(currentTimelineIndex);
            if (resetResult.has_value()) return false; // FATAL
            return true;
        }

        // Pool is full- > GPU is still executing commands from it
        return false;
    });

    // No pools found, creating a new one
    if (it == commandPools.end()) {
        auto result = createCommandPool();
        if (!result.isOk()) {
            return CommandBufferResult::err(result.unwrapError());
        }
        auto commandPool = std::move(result.unwrap());
        std::shared_ptr<vk::raii::CommandBuffer> commandBuffer = (*commandPool)[0];

        commandPools.push_back({
            .commandPool = std::move(commandPool),
            .lastSubmittedTimelineIndex = 0,
            .lastIdx = 1,
            .beingUsed = true
        });

        return CommandBufferResult::ok(std::make_unique<VulkanCommandBuffer>(
            device.weak_from_this(), 1, std::move(commandBuffer)));
    }

    // pool found

    auto commandBuffer = it->getAvailableCommandBuffer();
    return CommandBufferResult::ok(std::make_unique<VulkanCommandBuffer>(
        device.weak_from_this(), it->lastIdx, std::move(commandBuffer)));
}

void VulkanDevice::CommandPoolManager::releaseCommandBuffer(const uint32_t handle) {
    if (handle >= commandPools.size()) return;
    commandPools[handle].beingUsed = false;
}

VulkanDevice::CommandPoolManager::CommandPoolResult VulkanDevice::CommandPoolManager::createCommandPool() const {
    auto result = VulkanCommandPool::create(device.weak_from_this(), device.getQueueFamilyIndex());
    if (!result.isOk()) {
        return CommandPoolResult::err(result.unwrapError());
    }
    return CommandPoolResult::ok(std::move(result.unwrap()));
}

std::shared_ptr<vk::raii::CommandBuffer> VulkanDevice::CommandPoolManager::CommandPoolEntry::getAvailableCommandBuffer() {
    return (*commandPool)[++lastIdx];
}

std::optional<VulkanError> VulkanDevice::CommandPoolManager::CommandPoolEntry::reset(
    uint64_t currentCompletedTimelineIndex) {
    if (beingUsed) return VulkanError::ResetPoolInUse;
    if (lastSubmittedTimelineIndex > currentCompletedTimelineIndex)
        return VulkanError::TooEarlyToResetPool;

    if (auto resetResult = commandPool->reset()) {
        return resetResult;
    }

    lastIdx = 0;
    return std::nullopt;
}

}