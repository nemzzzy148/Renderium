//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <cstddef>
#include <vector>

namespace renderium {
enum class TextureFormat;
class ShaderModule;

struct ColorTargetState {
    TextureFormat format;
};

struct FragmentState {
    const ShaderModule& module;
    std::string entryPoint;
    std::vector<ColorTargetState> targets;
};

}
