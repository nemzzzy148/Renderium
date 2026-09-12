//
// Created by Nemesis Verstraete on 06/09/2026.
//

#include "ShaderCompiler.h"
#include "Backend.h"
#include "slang/SlangCompiler.h"

namespace shader {

ShaderCompiler::CompilerResult ShaderCompiler::create(const renderium::Backend backend) {
    switch (backend) {
        case renderium::Backend::Vulkan: {
            return std::unique_ptr<ShaderCompiler>(new SlangCompiler());
        }
        default: return CompilerResult::err(renderium::Error::FailedToCreateShaderCompiler);
    }
}

}
