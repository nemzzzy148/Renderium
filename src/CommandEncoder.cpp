//
// Created by Nemesis Verstraete on 08/11/2026.
//

#include "CommandEncoder.h"
#include "Pass.h"
#include "CommandBuffer.h"

namespace renderium {

RenderPass CommandEncoder::beginRenderPass(const RenderPassDescriptor &descriptor) const {
    if (!impl) return RenderPass(nullptr);
    return RenderPass(impl->beginRenderPass(descriptor));
}

CommandBuffer CommandEncoder::finish() const {
    return finish({});
}

CommandBuffer CommandEncoder::finish(const CommandBufferDescriptor &descriptor) const {
    return CommandBuffer(impl->finish(descriptor));
}

}
