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

}
