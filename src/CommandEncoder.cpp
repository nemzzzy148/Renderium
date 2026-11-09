//
// Created by Nemesis Verstraete on 08/11/2026.
//

#include "CommandEncoder.h"
#include "Pass.h"

namespace renderium {

RenderPass CommandEncoder::beginRenderPass(const RenderPassDescriptor &descriptor) const {
    return RenderPass(nullptr);
}

}
