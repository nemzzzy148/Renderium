//
// Created by Nemesis Verstraete on 09/11/2026.
//

#pragma once
#include <algorithm>

#include "CommandBuffer.h"

namespace rhi {

template<typename Api>
class CommandBufferImpl : public renderium::CommandBuffer::Impl {
public:
    using CommandBuffer = Api::CommandBuffer;
private:
    explicit CommandBufferImpl(CommandBuffer commandBuffer) : commandBuffer(std::move(commandBuffer)) {}
    CommandBuffer commandBuffer;
};

}
