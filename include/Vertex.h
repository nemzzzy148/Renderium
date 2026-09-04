//
// Created by Nemesis Verstraete on 12/08/2026.
//

#pragma once
#include <cstddef>
#include <string>
#include "Shader.h"

namespace renderium {
class Shader;

enum class VertexFormat {
    Float32,
    Float32x2,
    Float32x3
};

struct VertexAttribute {
    VertexFormat format;
    size_t offset;
};

struct VertexLayout {
    size_t vertexStride;
    size_t attributeCount;
    VertexAttribute attributes[];
};

struct VertexState {
    Shader& shader;
    EntryPointType entryPointType = EntryPointType::AttributeIndication;
    // only necessary with function name entry point indication
    std::string functionEntryPointName = "";
    size_t layoutCount;
    VertexLayout layouts[];
};

}
