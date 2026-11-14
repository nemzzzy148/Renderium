//
// Created by Nemesis Verstraete on 09/11/2026.
//

#pragma once
#include <algorithm>

#include "CommandBuffer.h"
#include "CommandEncoderImpl.h"
#include "DeviceState.h"

namespace rhi {

template<typename Api>
class CommandBufferImpl : public renderium::CommandBuffer::Impl {
    using CommandBuffer = Api::CommandBuffer;
    explicit CommandBufferImpl(std::shared_ptr<DeviceState<Api>> deviceState,
        std::unique_ptr<CommandBuffer> commandBuffer, CommandCollection collection)
        : deviceState(std::move(deviceState)), commandBuffer(std::move(commandBuffer))
        , collection(std::move(collection)) {}

    std::shared_ptr<DeviceState<Api>> deviceState;
    std::unique_ptr<CommandBuffer> commandBuffer;

    CommandCollection collection;
};

}
