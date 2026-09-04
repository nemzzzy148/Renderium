//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <cstddef>

namespace renderium {
enum class TextureFormat;
class Shader;

struct ColorAttachmentState {
    TextureFormat format;
};

struct FragmentState {
    Shader& shader;
    size_t targetCount;
    ColorAttachmentState targets[];
};

}