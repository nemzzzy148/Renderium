//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include <functional>
#include <utility>

#include "Result.h"
#include "VulkanDevice.h"
#include "../Error.h"

namespace renderium {
struct BufferDescriptor;
}

namespace rhi::vulkan {

class VulkanBuffer {
public:
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VulkanBuffer(VulkanBuffer&& other) noexcept
        : destructCallback(std::move(other.destructCallback)), device(std::move(other.device)),
          poolIdx(other.poolIdx), nodeIdx(other.nodeIdx), memoryType(other.memoryType) {}
    VulkanBuffer& operator=(VulkanBuffer&&) = delete;

    ~VulkanBuffer();
private:
    explicit VulkanBuffer(std::shared_ptr<VulkanDevice> device,
        std::function<void(uint32_t, uint32_t, uint32_t)> destructCallback, const MemoryAllocation& allocation)
        : destructCallback(std::move(destructCallback)), device(std::move(device)),
        poolIdx(allocation.poolIndex), nodeIdx(allocation.nodeIndex), memoryType(allocation.memoryType) {}

    std::function<void(uint32_t, uint32_t, uint32_t)> destructCallback;
    std::shared_ptr<VulkanDevice> device;

    const uint32_t poolIdx, nodeIdx, memoryType;

    friend class VulkanDevice;
};

}
