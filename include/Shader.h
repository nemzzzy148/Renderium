//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once
#include "ShaderCompiler.h"

namespace rhi {
template<typename Api>
class ShaderImpl;
}

namespace renderium {

class Shader {
    explicit Shader(shader::CompiledShader compiledShader) : compiledShader(std::move(compiledShader)) {}
    shader::CompiledShader compiledShader;
};

}