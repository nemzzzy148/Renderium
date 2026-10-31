//
// Created by Nemesis Verstraete on 22/10/2026.
//

#include "ShaderCompilerImpl.h"

#include "API.h"
#include "../shader/slang/SlangCompiler.h"

namespace rhi {

IShaderCompiler::ShaderCompilerResult IShaderCompiler::create(const renderium::ShadingLanguage language,
    const renderium::shader::ShadingOutputLanguage outputLanguage) {
    switch (language) {
        case renderium::ShadingLanguage::SLANG: {
            return ShaderCompilerImpl<shader::ShaderCompilerType<renderium::ShadingLanguage::SLANG>>
                ::create(outputLanguage);
        }
    }
    return ShaderCompilerResult::err(renderium::RequestError::UnknownShadingLanguage);
}

template<typename Api>
ShaderCompilerImpl<Api>::ShaderCompilerResult ShaderCompilerImpl<Api>::create(
        renderium::shader::ShadingOutputLanguage outputLanguage) {
    auto result = ShaderCompiler::create(outputLanguage);
    if (!result.isOk()) {
        return ShaderCompilerResult::err(renderium::RequestError::FailedToCreateShaderCompiler);
    }
    return ShaderCompilerResult::ok(
        std::unique_ptr<IShaderCompiler>(new ShaderCompilerImpl(std::move(result.unwrap()))));
}

template<typename Api>
ShaderCompilerImpl<Api>::ShaderResult ShaderCompilerImpl<Api>::compileShader(DeviceErrorState &errorState,
        const std::string &shaderCode) {
    auto result = shaderCompiler.compileShader(shaderCode);
    if (!result.isOk()) {
        errorState.report({renderium::ErrorFilter::Validation, "Shader compilation failed."});
        return ShaderResult::err(renderium::RequestError::ShaderCompilationError);
    }
    return ShaderResult::ok(std::move(result.unwrap()));
}

}
