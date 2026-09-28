//
// Created by Nemesis Verstraete on 12/08/2026.
//

#pragma once

#include <string>
#include "Shader.h"

namespace renderium {
class Shader;

enum class VertexFormat {
    Float32,
    Float32x2,
    Float32x3,
    Float32x4
};

struct VertexAttribute {
    VertexFormat format;
    size_t offset;
    uint32_t location;
};

enum class VertexStepMode {
    Vertex,
    Instance
};

struct VertexLayout {
    uint32_t binding;
    size_t vertexStride;
    VertexStepMode stepMode;
    std::span<const VertexAttribute> attributes;
};

struct VertexState {
    Shader& shader;
    std::string functionEntryPointName;
    std::span<const VertexLayout> layouts;
};

}
