//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

#include <memory>

#include "Fragment.h"
#include "Primitive.h"
#include "Resource.h"
#include "Vertex.h"

namespace resource {
template<typename T>
class ResourceManager;
}

namespace rhi {
class IRenderPipeline;
}

namespace renderium {
enum class RequestError;

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
    explicit RenderPipeline(std::shared_ptr<detail::Resource> resource);

    std::shared_ptr<detail::Resource> resource;
    friend class Device;
    friend class RenderPass;
};

struct ComputePipelineDescriptor {

};

class ComputePipeline {

};

}
