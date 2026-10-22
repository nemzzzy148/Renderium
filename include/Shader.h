//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace rhi {
template<typename Api>
class RenderPipelineImpl;
}

namespace renderium {

enum class ShadingLanguage {
    SLANG
};

namespace shader {

enum class ShadingOutputLanguage {
    SPIRV,
    HLSL,
    GLSL,
    MSL
};

enum class ShaderStage {
    Fragment,
    Vertex,
    Compute
};

struct ShaderEntryPoint {
    ShaderStage stage;
    std::string name;
    std::string compiledName;
    std::vector<uint8_t> code;
};

struct CompiledShader {
    bool valid = false;
    ShadingOutputLanguage language = ShadingOutputLanguage::SPIRV;
    std::vector<ShaderEntryPoint> entryPoints;
};

}

struct ShaderModuleDescriptor {
    std::string code;
};

class ShaderModule {
public:
    [[nodiscard]] const shader::CompiledShader& getCompiledShader() const { return compiledShader; }

    [[nodiscard]] bool isValid() const { return compiledShader.valid; }
private:
    explicit ShaderModule(shader::CompiledShader compiledShader) : compiledShader(std::move(compiledShader)) {}
    shader::CompiledShader compiledShader;

    friend class Device;
    template<typename Api>
    friend class rhi::RenderPipelineImpl;
};

}