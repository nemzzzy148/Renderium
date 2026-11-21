//
// Created by Nemesis Verstraete on 14/11/2026.
//

#include "CommandBufferImpl.h"

#include "API.h"

namespace rhi {
template<typename Api>
void CommandBufferImpl<Api>::begin() {
    commandBuffer->begin();
}

template<typename Api>
CommandBufferImpl<Api>::CommandBuffer & CommandBufferImpl<Api>::getNativeCommandBuffer() {
    return *commandBuffer;
}

template<typename Api>
void CommandBufferImpl<Api>::end() {
    commandBuffer->end();
}

template class CommandBufferImpl<VulkanApi>;

}