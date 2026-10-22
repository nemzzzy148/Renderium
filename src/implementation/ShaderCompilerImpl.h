//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <memory>
#include <string>
#include <Error.h>

#include "Result.h"
#include "Shader.h"
#include "DeviceErrorState.h"

namespace renderium {
enum class ShadingLanguage;
}

namespace rhi {

struct IShaderCompiler {
    virtual ~IShaderCompiler() = default;

    using ShaderCompilerResult = renderium::Result<std::unique_ptr<IShaderCompiler>, renderium::RequestError>;
    static ShaderCompilerResult create(renderium::ShadingLanguage language,
        renderium::shader::ShadingOutputLanguage outputLanguage);

    virtual renderium::Result<renderium::shader::CompiledShader, renderium::RequestError> compileShader(
        DeviceErrorState& errorState, const std::string& shaderCode) = 0;
};

template<typename Api>
class ShaderCompilerImpl : public IShaderCompiler {
public:
    using ShaderCompiler = Api::ShaderCompiler;

    using ShaderCompilerResult = renderium::Result<std::unique_ptr<IShaderCompiler>, renderium::RequestError>;
    static ShaderCompilerResult create(renderium::shader::ShadingOutputLanguage outputLanguage) {
        auto result = ShaderCompiler::create(outputLanguage);
        if (!result.isOk()) {
            return ShaderCompilerResult::err(renderium::RequestError::FailedToCreateShaderCompiler);
        }
        return ShaderCompilerResult::ok(
            std::unique_ptr<IShaderCompiler>(new ShaderCompilerImpl(std::move(result.unwrap()))));
    }

    using ShaderResult = renderium::Result<renderium::shader::CompiledShader, renderium::RequestError>;
    ShaderResult compileShader(DeviceErrorState& errorState, const std::string &shaderCode) override {
        auto result = shaderCompiler.compileShader(shaderCode);
        if (!result.isOk()) {
            errorState.report({renderium::ErrorFilter::Validation, "Shader compilation failed."});
            return ShaderResult::err(renderium::RequestError::ShaderCompilationError);
        }
        return ShaderResult::ok(std::move(result.unwrap()));
    }
    explicit ShaderCompilerImpl(ShaderCompiler shaderCompiler) : shaderCompiler(std::move(shaderCompiler)) {}

    ~ShaderCompilerImpl() override = default;
private:
    ShaderCompiler shaderCompiler;
};

}