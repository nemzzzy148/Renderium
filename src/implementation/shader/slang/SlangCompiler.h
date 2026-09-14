//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

#include "Error.h"
#include "Result.h"

namespace renderium {
enum class Backend;
}

namespace rhi::shader::slang {
enum class SlangError;

class SlangCompiler {
public:
    using ShaderCompilerResult = renderium::Result<SlangCompiler, SlangError>;
    static ShaderCompilerResult create(renderium::Backend backend);

    using ShaderResult = renderium::Result<std::string, SlangError>;
    ShaderResult compileShader(const std::string& shaderCode);
private:
    explicit SlangCompiler(Slang::ComPtr<::slang::IGlobalSession> globalSession,
        Slang::ComPtr<::slang::ISession> session, const renderium::Backend backend)
        : globalSession(std::move(globalSession)), session(std::move(session)), backend(backend) {}
    Slang::ComPtr<::slang::IGlobalSession> globalSession;
    Slang::ComPtr<::slang::ISession> session;
    renderium::Backend backend;
};

}
