//
// Created by Nemesis Verstraete on 30/08/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"
#include "../Error.h"

namespace rhi::vulkan {
class VulkanDevice;

class VulkanQueue {
public:
    using QueueResult = renderium::Result<VulkanQueue, VulkanError>;
    static QueueResult create(const VulkanDevice& device) {
        return QueueResult::ok(VulkanQueue(device));
    }
private:
    explicit VulkanQueue(const VulkanDevice& device);
    vk::raii::Queue queue = nullptr;
};

}
