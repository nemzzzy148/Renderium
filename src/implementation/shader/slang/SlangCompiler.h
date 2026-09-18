//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <optional>
#include <string_view>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <vector>
#include <__filesystem/filesystem_error.h>

#include "Result.h"

namespace renderium {
namespace shader {
enum class ShadingOutputLanguage;
enum class ShaderEntryPointType;
struct CompiledShader;
}

enum class Backend;
}

namespace rhi::shader::slang {
enum class SlangError;

class SlangCompiler {
public:
    using ShaderCompilerResult = renderium::Result<SlangCompiler, SlangError>;
    static ShaderCompilerResult create(renderium::shader::ShadingOutputLanguage outputLanguage);
private:
    Slang::ComPtr<::slang::IModule> loadModuleFromString(const std::string& shaderCode, Slang::ComPtr<::slang::IBlob>& diagnostics) const;
    static std::vector<::slang::IEntryPoint*> discoverAllEntryPoints(::slang::IModule* module);
    static std::optional<renderium::shader::ShaderEntryPointType> classifyStage(SlangStage stage);
public:
    using ShaderResult = renderium::Result<renderium::shader::CompiledShader, SlangError>;
    [[nodiscard]] ShaderResult compileShader(const std::string& shaderCode) const;
private:
    explicit SlangCompiler(Slang::ComPtr<::slang::IGlobalSession> globalSession,
        Slang::ComPtr<::slang::ISession> session, const renderium::shader::ShadingOutputLanguage compileLanguage)
        : globalSession(std::move(globalSession)), session(std::move(session)), compileLanguage(compileLanguage) {}
    Slang::ComPtr<::slang::IGlobalSession> globalSession;
    Slang::ComPtr<::slang::ISession> session;
    renderium::shader::ShadingOutputLanguage compileLanguage;
};

}
