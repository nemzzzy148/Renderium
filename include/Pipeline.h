//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include "Fragment.h"
#include "Vertex.h"

namespace renderium {

struct PipelineLayoutCreateInfo {

};

class PipelineLayout {

};

struct RenderPipelineCreateInfo {
    PipelineLayout pipelineLayout;
    FragmentState fragmentState;
    VertexState vertexState;
};

class RenderPipeline {

};

struct ComputePipelineCreateInfo {

};

class ComputePipeline {

};

}
