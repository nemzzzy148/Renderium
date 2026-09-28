//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <cstddef>
#include <vector>

namespace renderium {
enum class TextureFormat;
class Shader;

struct ColorAttachmentState {
    TextureFormat format;
};

struct FragmentState {
    Shader& shader;
    std::string functionEntryPointName;
    std::span<const ColorAttachmentState> targets;
};

}
