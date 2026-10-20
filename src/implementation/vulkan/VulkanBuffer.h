//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include <functional>

#include "Result.h"
#include "VulkanDevice.h"
#include "../Error.h"

namespace renderium {
struct BufferDescriptor;
}

namespace rhi::vulkan {

class VulkanBuffer {
public:
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
