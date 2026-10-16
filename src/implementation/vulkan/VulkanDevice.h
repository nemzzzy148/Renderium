//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <memory>
#include <utility>
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../DeviceImpl.h"
#include "../API.h"
#include "../../memory/TLSF.h"

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
    uint32_t pool;
    uint32_t node;
};

class VulkanDevice {
    // --- device ---
    using QueueIndexResult = renderium::Result<uint32_t, VulkanError>;
    static QueueIndexResult getQueueIndex(vk::QueueFlagBits flags, const vk::raii::PhysicalDevice& physicalDevice);
    using VulkanDeviceResult = renderium::Result<std::pair<vk::raii::Device, uint32_t>, VulkanError>;
    static VulkanDeviceResult createDevice(const vk::raii::PhysicalDevice& physicalDevice,
        const renderium::DeviceDescriptor& descriptor, bool surfaceSupport);
public:
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
          queueFamilyIndex(familyQueueIndex), device(std::move(device)) {}

    struct MemoryManagerDescriptor {
        uint32_t bufferAlignment = 1 << 8;
        uint32_t textureAlignment = 1 << 8; // this is most of the time bigger
    };
    class MemoryManager {
    public:
        explicit MemoryManager(MemoryManagerDescriptor descriptor);

        MemoryAllocation bufferAllocation(uint64_t size);
        MemoryAllocation textureAllocation(uint64_t size);

        void freeTexture(uint32_t index);
        void freeBuffer(uint32_t index);

        static constexpr uint64_t DEFAULT_SIZE = 1<<24;
    private:
        const uint32_t bufferAlignment;
        std::vector<vk::raii::DeviceMemory> bufferMemory;
        TLSF bufferTlsf;

        const uint32_t textureAlignment;
        std::vector<vk::raii::DeviceMemory> textureMemory;
        TLSF textureTlsf;
    };

    std::shared_ptr<VulkanInstance> instance;
    std::shared_ptr<vk::raii::PhysicalDevice> physicalDevice;
    uint32_t queueFamilyIndex;
    vk::raii::Device device;
    friend class VulkanQueue;
};

}
