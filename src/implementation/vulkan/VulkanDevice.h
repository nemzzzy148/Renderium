//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <memory>
#include <unordered_map>
#include <utility>
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../DeviceImpl.h"
#include "../API.h"
#include "../../memory/TLSF.h"
#include "../../utils/Hashes.h"

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

struct MemoryAllocation {
    uint64_t size = 0;
    uint32_t poolIndex = 0;
    uint32_t nodeIndex = 0;
    uint32_t memoryType = 0; // Warning: this is a valid memory type
    bool valid = false;
};

class VulkanDevice {
    // --- device ---
    using QueueIndexResult = renderium::Result<uint32_t, VulkanError>;
    static QueueIndexResult getQueueIndex(vk::QueueFlagBits flags, const vk::raii::PhysicalDevice& physicalDevice);
    using VulkanDeviceResult = renderium::Result<std::pair<vk::raii::Device, uint32_t>, VulkanError>;
    static VulkanDeviceResult createDevice(const vk::raii::PhysicalDevice& physicalDevice,
        const renderium::DeviceDescriptor& descriptor, bool surfaceSupport);

    class MemoryManager {
    public:
        explicit MemoryManager(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device);

        MemoryAllocation bufferAllocation(uint32_t alignment, uint64_t size,
            uint32_t typeFilter, vk::MemoryPropertyFlags memoryProperties);
        MemoryAllocation textureAllocation(uint32_t alignment, uint64_t size,
            uint32_t typeFilter, vk::MemoryPropertyFlags memoryProperties);

        void freeBuffer(int32_t poolIndex, uint32_t nodeIndex, uint32_t memoryType);
        void freeTexture(uint32_t poolIndex, uint32_t nodeIndex, uint32_t memoryType);

        static constexpr uint64_t DEFAULT_SIZE = 1<<24;
    private:
        std::optional<uint32_t> findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

        using DeviceMemoryResult = renderium::Result<vk::raii::DeviceMemory, VulkanError>;
        DeviceMemoryResult allocateDeviceMemory(uint64_t size, uint32_t memoryType) const;

        struct Heap {
            // first uint for poolIdx, second for nodeIdx
            std::unordered_map<std::pair<uint32_t, uint32_t>, vk::raii::DeviceMemory, PairHash> memory;
            TLSF tlsf;
        };

        MemoryAllocation allocate(Heap& heap, uint32_t alignment, uint64_t size, uint32_t memoryType) const;

        const vk::raii::PhysicalDevice& physicalDevice;
        const vk::raii::Device& device;

        std::unordered_map<uint32_t, Heap> bufferHeaps;
        std::unordered_map<uint32_t, Heap> textureHeaps;
    };
public:
    const MemoryManager memoryManager;

    using DeviceResult = renderium::Result<VulkanDevice, VulkanError>;
    static DeviceResult create(std::shared_ptr<VulkanInstance> instance,
        std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice,
        const renderium::DeviceDescriptor& descriptor, bool surfaceSupport);
    [[nodiscard]] const vk::raii::PhysicalDevice& getPhysicalDevice() const { return *physicalDevice; }
    [[nodiscard]] const vk::raii::Device& getDevice() const { return device; }
    [[nodiscard]] uint32_t getQueueFamilyIndex() const { return queueFamilyIndex; }
private:
    VulkanDevice(std::shared_ptr<VulkanInstance> instance,
        std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice, vk::raii::Device device,
        const uint32_t familyQueueIndex)
        : instance(std::move(instance)), physicalDevice(std::move(physicalDevice)),
          queueFamilyIndex(familyQueueIndex), device(std::move(device)), memoryManager(*physicalDevice, device){}

    std::shared_ptr<VulkanInstance> instance;
    std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice;
    uint32_t queueFamilyIndex;
    vk::raii::Device device;

    friend class VulkanQueue;
};

}
