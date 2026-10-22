//
// Created by Nemesis Verstraete on 13/09/2026.
//

#pragma once
#include <optional>
#include <string>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <vector>

#include "Result.h"
#include "Shader.h"

namespace rhi::shader::slang {
enum class SlangError;

class SlangCompiler {
public:
    using ShaderCompilerResult = renderium::Result<SlangCompiler, SlangError>;
    static ShaderCompilerResult create(renderium::shader::ShadingOutputLanguage outputLanguage);
private:
    Slang::ComPtr<::slang::IModule> loadModuleFromString(const std::string& shaderCode, Slang::ComPtr<::slang::IBlob>& diagnostics) const;
    static std::vector<::slang::IEntryPoint*> discoverAllEntryPoints(::slang::IModule* module);
    static std::optional<renderium::shader::ShaderStage> classifyStage(SlangStage stage);
    static std::string compiledEntryPointName(renderium::shader::ShadingOutputLanguage shadingOutputLanguage,
        const std::string& sourceName);
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
