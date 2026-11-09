//
// Created by Nemesis Verstraete on 08/11/2026.
//

#include "Pass.h"

namespace renderium {

RenderPass::~RenderPass() { end(); }

void RenderPass::setPipeline(const RenderPipeline &pipeline) const {

}

void RenderPass::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {

}

void RenderPass::end() const {

}

}