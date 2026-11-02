//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "VulkanQueue.h"

#include "VulkanDevice.h"

namespace rhi::vulkan {

VulkanQueue::QueueResult VulkanQueue::create(const VulkanDevice &device) {
    return QueueResult::ok(VulkanQueue(device.getDevice().getQueue(device.getQueueFamilyIndex(), 0)));
}

VulkanQueue::VulkanQueue(vk::raii::Queue queue) :  queue(std::move(queue)) {}

}