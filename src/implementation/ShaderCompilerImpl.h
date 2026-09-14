//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <algorithm>
#include <Error.h>

#include "Result.h"
#include "Shader.h"

namespace renderium {
enum class Backend;
}

namespace rhi {

template<typename Api>
class ShaderCompilerImpl : public renderium::ShaderCompiler::Impl {
public:
    using ShaderCompiler = Api::ShaderCompiler;

    using ShaderCompilerResult = renderium::Result<std::unique_ptr<ShaderCompilerImpl>, renderium::Error>;
    static ShaderCompilerResult create(const renderium::Backend backend) {
        auto result = ShaderCompiler::create(backend);
        if (!result.isOk()) {
            return ShaderCompilerResult::err(renderium::Error::FailedToCreateShaderCompiler);
        }
        return ShaderCompilerResult::ok(
            std::unique_ptr<ShaderCompilerImpl>(new ShaderCompilerImpl(std::move(result.unwrap))));
    }
    explicit ShaderCompilerImpl(ShaderCompiler shaderCompiler) : shaderCompiler(std::move(shaderCompiler)) {}

    renderium::Result<std::string, renderium::Error> compileShader(const std::string &shaderCode) override {
        auto result = shaderCompiler.compileShader(shaderCode);
    }
private:
    ShaderCompiler shaderCompiler;
};

}
