//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include "Result.h"
#include "VulkanDevice.h"
#include "../Error.h"

namespace renderium {
struct BufferDescriptor;
}

namespace rhi::vulkan {

class VulkanBuffer {
public:
    using BufferResult = renderium::Result<VulkanBuffer, VulkanError>;
    static BufferResult create(const renderium::BufferDescriptor& descriptor);
private:
    explicit VulkanBuffer(MemoryAllocation allocation);

    MemoryAllocation allocation;
    std::shared_ptr<VulkanDevice> device;
};

}
