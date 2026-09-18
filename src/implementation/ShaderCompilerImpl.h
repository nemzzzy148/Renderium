//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <algorithm>
#include <Error.h>

#include "Result.h"
#include "ShaderCompiler.h"

namespace renderium {
enum class Backend;
}

namespace rhi {

template<typename Api>
class ShaderCompilerImpl : public renderium::shader::ShaderCompiler::Impl {
public:
    using ShaderCompiler = Api::ShaderCompiler;

    using ShaderCompilerResult = renderium::Result<std::unique_ptr<ShaderCompilerImpl>, renderium::Error>;
    static ShaderCompilerResult create(const renderium::shader::ShadingOutputLanguage outputLanguage) {
        auto result = ShaderCompiler::create(outputLanguage);
        if (!result.isOk()) {
            return ShaderCompilerResult::err(renderium::Error::FailedToCreateShaderCompiler);
        }
        return ShaderCompilerResult::ok(
            std::unique_ptr<ShaderCompilerImpl>(new ShaderCompilerImpl(std::move(result.unwrap()))));
    }
    explicit ShaderCompilerImpl(ShaderCompiler shaderCompiler) : shaderCompiler(std::move(shaderCompiler)) {}

    using ShaderResult = renderium::Result<renderium::shader::CompiledShader, renderium::Error>;
    ShaderResult compileShader(const std::string &shaderCode) override {
        auto result = shaderCompiler.compileShader(shaderCode);
        if (!result.isOk()) {
            return ShaderResult::err(renderium::Error::ShaderCompilationError);
        }
        return ShaderResult::ok(std::move(result.unwrap()));
    }

    ~ShaderCompilerImpl() override = default;
private:
    ShaderCompiler shaderCompiler;
};

}