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
template<typename Api>
class DeviceImpl;
}

namespace renderium {
enum class Error;

// for descriptors, IMPLEMENT

struct PipelineLayoutDescriptor {

};

class PipelineLayout {

};

struct RenderPipelineDescriptor {
    //PipelineLayout pipelineLayout;
    VertexState vertex;
    FragmentState fragment;
    PrimitiveState primitive;

};

class RenderPipeline {
    struct Impl {
        ~Impl() = default;
    };

    explicit RenderPipeline(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    friend class Device;
    template<typename Api>
    friend class rhi::RenderPipelineImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

struct ComputePipelineDescriptor {

};

class ComputePipeline {

};

}
