//
// Created by Nemesis Verstraete on 13/09/2026.
//

#include "SlangCompiler.h"

#include <string>

#include "Backend.h"
#include "../../Error.h"

namespace rhi::shader::slang {

SlangCompiler::ShaderCompilerResult SlangCompiler::create(const renderium::Backend backend) {
    Slang::ComPtr<::slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(::slang::createGlobalSession(globalSession.writeRef())))
        return ShaderCompilerResult::err(SlangError::FailedToCreateGlobalSession);

    SlangCompileTarget target = SLANG_SPIRV;
    std::string profileName;
    switch (backend) {
        case renderium::Backend::Vulkan: {
            target = SLANG_SPIRV;
            profileName = "spirv_1_6";
            break;
        }
    }
    ::slang::TargetDesc targetDesc = {
        .format = target,
        .profile = globalSession->findProfile(profileName.c_str())
    };

    Slang::ComPtr<::slang::ISession> session;
    const ::slang::SessionDesc sessionDesc = {
        .targets = &targetDesc,
        .targetCount = 1
    };
    if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef())))
        return ShaderCompilerResult::err(SlangError::FailedToCreateSession);

    return ShaderCompilerResult::ok(
        SlangCompiler(std::move(globalSession), std::move(session), backend));
}

SlangCompiler::ShaderResult SlangCompiler::compileShader(const std::string& shaderCode) {

}

}
