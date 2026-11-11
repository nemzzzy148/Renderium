//
// Created by Nemesis Verstraete on 08/11/2026.
//

#include "Pass.h"

#include "Pipeline.h"
#include "implementation/PassImpl.h"

namespace renderium {

RenderPass::~RenderPass() { end(); }

void RenderPass::setPipeline(const RenderPipeline &pipeline) const {
    if (!impl) return;
    impl->setPipeline(*pipeline.resource);
}

void RenderPass::draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
        const uint32_t firstInstance) const {
    if (!impl) return;
    impl->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void RenderPass::end() const {
    if (!impl) return;
    impl->end();
}

}
