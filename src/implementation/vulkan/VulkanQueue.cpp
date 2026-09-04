//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "VulkanQueue.h"

#include "VulkanDevice.h"

namespace rhi::vulkan {

VulkanQueue::VulkanQueue(const VulkanDevice& device)
    : queue(vk::raii::Queue(device.device, device.getQueueFamilyIndex(), 0)) {}

}