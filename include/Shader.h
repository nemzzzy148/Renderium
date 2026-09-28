//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once
#include "ShaderCompiler.h"

namespace rhi {
template<typename Api>
class PipelineImpl;
}

namespace renderium {

class Shader {
public:
    const shader::CompiledShader& getCompiledShader() const { return compiledShader; }
private:
    explicit Shader(shader::CompiledShader compiledShader) : compiledShader(std::move(compiledShader)) {}
    shader::CompiledShader compiledShader;

    friend class Device;
    template<typename Api>
    friend class rhi::PipelineImpl;
};

}