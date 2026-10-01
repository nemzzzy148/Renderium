//
// Created by Nemesis Verstraete on 12/08/2026.
//

#pragma once

#include <string>
#include "Shader.h"

namespace renderium {
class ShaderModule;

enum class VertexFormat {
    Float32,
    Float32x2,
    Float32x3,
    Float32x4
};

struct VertexAttribute {
    VertexFormat format;
    size_t offset;
    uint32_t shaderLocation;
};

enum class VertexStepMode {
    Vertex,
    Instance
};

struct VertexBufferLayout {
    size_t arrayStride;
    VertexStepMode stepMode;
    std::vector<VertexAttribute> attributes;
};

struct VertexState {
    const ShaderModule& module;
    std::string entryPoint;
    std::vector<VertexBufferLayout> buffers;
};

}
