//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

#include <memory>

#include "Fragment.h"
#include "Primitive.h"
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

// for descriptors, IMPLEMENT

struct PipelineLayoutCreateInfo {

};

class PipelineLayout {

};

struct RenderPipelineCreateInfo {
    //PipelineLayout pipelineLayout;
    VertexState vertexState;
    FragmentState fragmentState;
    PrimitiveState primitiveState;

};

class RenderPipeline {
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
