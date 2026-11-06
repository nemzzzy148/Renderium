//
// Created by Nemesis Verstraete on 03/11/2026.
//

#pragma once
#include "Pass.h"

namespace rhi {

template<typename Api>
class RenderPassImpl : renderium::RenderPass::Impl {
public:
    using RenderPass = Api::RenderPass;
private:
    explicit RenderPassImpl(RenderPass renderPass) : renderPass(std::move(renderPass)) {}
    RenderPass renderPass;
};

}
