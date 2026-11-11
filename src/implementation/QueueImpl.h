//
// Created by Nemesis Verstraete on 30/08/2026.
//

#pragma once

#include "CommandBufferImpl.h"
#include "DeviceState.h"
#include "../track/UsageTracker.h"
#include "vulkan/VulkanQueue.h"
#include "DeviceImpl.h"
#include "Queue.h"
#include "../synchronization/TimelineCounter.h"

namespace rhi {

template<typename Api>
class QueueImpl : public renderium::Queue::Impl {
    using Queue = Api::Queue;
    explicit QueueImpl(Queue queue, std::shared_ptr<DeviceState<Api>> deviceState)
        : queue(std::move(queue)), deviceState(std::move(deviceState)) {}
    Queue queue;
    std::shared_ptr<DeviceState<Api>> deviceState;

    synchronization::TimelineCounter timeline;

    std::queue<CommandBufferImpl<Api>> commands;

    friend class DeviceImpl<Api>;
};

}