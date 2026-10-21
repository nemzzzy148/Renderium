//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once
#include <optional>
#include <string>

#include "ShaderCompiler.h"

namespace rhi {
template<typename Api>
class PipelineImpl;
}

namespace renderium {

struct ShaderModuleDescriptor {
    std::string code;
};

class ShaderModule {
public:
    [[nodiscard]] const shader::CompiledShader& getCompiledShader() const { return *compiledShader; }

    [[nodiscard]] bool isValid() const { return compiledShader.has_value(); }
private:
    explicit ShaderModule(shader::CompiledShader compiledShader) : compiledShader(std::move(compiledShader)) {}
    std::optional<shader::CompiledShader> compiledShader;

    friend class Device;
    template<typename Api>
    friend class rhi::PipelineImpl;
};

}