//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

#include <memory>

#include "Fragment.h"
#include "Result.h"
#include "Vertex.h"

namespace rhi {

template<typename Api>
class RenderPipelineImpl;
template<typename Api>
class ComputePipelineImpl;

}

namespace renderium {
enum class Error;

struct PipelineLayoutCreateInfo {

};

class PipelineLayout {

};

struct RenderPipelineCreateInfo {
    //PipelineLayout pipelineLayout;
    FragmentState fragmentState;
    VertexState vertexState;
};

class RenderPipeline {
public:
    using PipelineResult = Result<RenderPipeline, Error>;
    static PipelineResult create(const RenderPipelineCreateInfo& createInfo);
private:
    struct Impl {

    };

    explicit RenderPipeline(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::RenderPipelineImpl;
};

struct ComputePipelineCreateInfo {

};

class ComputePipeline {

};

}
