//
// Created by Nemesis Verstraete on 16/10/2026.
//

#include "VulkanBuffer.h"

namespace rhi::vulkan {

VulkanBuffer::~VulkanBuffer() {
    if (destructCallback) {
        destructCallback(poolIdx, nodeIdx, memoryType);
    }
}

}