//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Backend.h"
#include "Result.h"



namespace renderium {
enum class Error;
}

namespace rhi {
template<typename Api>
class ShaderCompilerImpl;
}

namespace renderium::shader {

enum class ShadingLanguage {
    SLANG
};

constexpr auto DefaultShadingLanguage = ShadingLanguage::SLANG;

enum class ShadingOutputLanguage {
    SPIRV,
    HLSL,
    GLSL,
    MSL
};

inline ShadingOutputLanguage apiToShadingLanguage(const Backend backend) {
    switch (backend) {
        case Backend::Vulkan: return ShadingOutputLanguage::SPIRV;
    }
    return ShadingOutputLanguage::SPIRV;
}

enum class ShaderEntryPointType {
    Fragment,
    Vertex,
    Compute
};

struct ShaderEntryPoint {
    ShaderEntryPointType entryPointType;
    std::string name;
    std::vector<uint8_t> code;
};

struct CompiledShader {
    ShadingOutputLanguage langauge;
    std::vector<ShaderEntryPoint> shaderEntryPoints;
};

class ShaderCompiler {
public:
    using ShaderCompilerResult = Result<ShaderCompiler, Error>;
    static ShaderCompilerResult create(ShadingLanguage language, ShadingOutputLanguage outputLanguage);

    using ShaderResult = Result<CompiledShader, Error>;
    [[nodiscard]] ShaderResult compileShader(const std::string& shaderCode) const;

    // for custom compilers
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<CompiledShader, Error> compileShader(const std::string& shaderCode);
    };
    explicit ShaderCompiler(std::unique_ptr<Impl> shaderCompiler) : impl(std::move(shaderCompiler)) {}
private:
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::ShaderCompilerImpl;

};

}