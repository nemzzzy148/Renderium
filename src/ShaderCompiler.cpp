//
// Created by Nemesis Verstraete on 18/09/2026.
//

#include "ShaderCompiler.h"

#include "implementation/API.h"
#include "implementation/ShaderCompilerImpl.h"
#include "implementation/shader/slang/SlangCompiler.h"


namespace renderium::shader {

ShaderCompiler::ShaderCompilerResult ShaderCompiler::create(const ShadingLanguage language,
    const ShadingOutputLanguage outputLanguage) {
    switch (language) {
        case ShadingLanguage::SLANG: {
            auto result = rhi::ShaderCompilerImpl<rhi::shader::SlangApi>::create(outputLanguage);
            if (!result.isOk()) {
                return ShaderCompilerResult::err(Error::FailedToCreateShaderCompiler);
            }
            return ShaderCompilerResult::ok(ShaderCompiler(std::move(result.unwrap())));
        }
    }
    return ShaderCompilerResult::err(Error::UnknownShadingLanguage);
}

ShaderCompiler::ShaderResult ShaderCompiler::compileShader(const std::string &shaderCode) const {
    return impl->compileShader(shaderCode);
}

Result<CompiledShader, Error> ShaderCompiler::Impl::compileShader(const std::string &) {
    return Result<CompiledShader, Error>::err(Error::ShaderCompilationError);
}

}