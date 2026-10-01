//
// Created by Nemesis Verstraete on 30/08/2026.
//

#pragma once

#include "vulkan/VulkanQueue.h"

namespace rhi {

template<typename Api>
class QueueImpl : public renderium::Queue::Impl {
    using Queue = Api::Queue;
    explicit QueueImpl(Queue queue) : queue(std::move(queue)) {}
    Queue queue;

    friend class DeviceImpl<Api>;
};

}