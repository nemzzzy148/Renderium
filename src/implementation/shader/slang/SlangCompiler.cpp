//
// Created by Nemesis Verstraete on 13/09/2026.
//

#include "SlangCompiler.h"

#include <filesystem>
#include <source_location>
#include <string>

#include "../../Error.h"
#include "ShaderCompiler.h"

namespace rhi::shader::slang {
SlangCompiler::ShaderCompilerResult SlangCompiler::create(const renderium::shader::ShadingOutputLanguage outputLanguage) {
    Slang::ComPtr<::slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(::slang::createGlobalSession(globalSession.writeRef())))
        return ShaderCompilerResult::err(SlangError::FailedToCreateGlobalSession);

    SlangCompileTarget target;
    std::string profileName;
    switch (outputLanguage) {
        case renderium::shader::ShadingOutputLanguage::SPIRV:
            target = SLANG_SPIRV;
            profileName = "spirv_1_6";
            break;
        case renderium::shader::ShadingOutputLanguage::HLSL:
            target = SLANG_HLSL;
            profileName = "sm_6_6";
            break;
        case renderium::shader::ShadingOutputLanguage::GLSL:
            target = SLANG_GLSL;
            profileName = "glsl_450";
            break;
        case renderium::shader::ShadingOutputLanguage::MSL:
            target = SLANG_METAL;
            profileName = "sm_6_6";
            break;
        default:
            return ShaderCompilerResult::err(SlangError::UnsupportedShadingLanguage);
    }
    ::slang::TargetDesc targetDesc = {
        .format = target,
        .profile = globalSession->findProfile(profileName.c_str())
    };
    if (targetDesc.profile == SLANG_PROFILE_UNKNOWN)
        return ShaderCompilerResult::err(SlangError::UnknownTargetProfile);

    Slang::ComPtr<::slang::ISession> session;
    const ::slang::SessionDesc sessionDesc = {
        .targets = &targetDesc,
        .targetCount = 1
    };
    if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef())))
        return ShaderCompilerResult::err(SlangError::FailedToCreateSession);

    return ShaderCompilerResult::ok(
        SlangCompiler(std::move(globalSession), std::move(session), outputLanguage));
}

Slang::ComPtr<::slang::IModule> SlangCompiler::loadModuleFromString(const std::string &shaderCode,
    Slang::ComPtr<::slang::IBlob> &diagnostics) const {
    return Slang::ComPtr(session->loadModuleFromSourceString("slang_shader", nullptr, shaderCode.c_str(), diagnostics.writeRef()));
}

std::vector<::slang::IEntryPoint*> SlangCompiler::discoverAllEntryPoints(::slang::IModule* module) {
    std::vector<::slang::IEntryPoint*> entryPoints;
    entryPoints.reserve(module->getDefinedEntryPointCount());
    for (int i = 0; i < module->getDefinedEntryPointCount(); i++) {
        ::slang::IEntryPoint* entryPoint = nullptr;
        module->getDefinedEntryPoint(i, &entryPoint);
        entryPoints.push_back(entryPoint);
    }
    return entryPoints;
}

std::optional<renderium::shader::ShaderEntryPointType> SlangCompiler::classifyStage(const SlangStage stage) {
    using T = renderium::shader::ShaderEntryPointType;
    switch (stage) {
        case SLANG_STAGE_VERTEX: return T::Vertex;
        case SLANG_STAGE_FRAGMENT: return T::Fragment;
        case SLANG_STAGE_COMPUTE: return T::Compute;
        default: return std::nullopt;
    }
}

SlangCompiler::ShaderResult SlangCompiler::compileShader(const std::string& shaderCode) const {
    Slang::ComPtr<::slang::IBlob> diagnostics;
    const Slang::ComPtr<::slang::IModule> module = loadModuleFromString(shaderCode, diagnostics);
    if (!module)
        return ShaderResult::err(SlangError::FailedToCreateSlangShaderModule);

    const auto entryPoints = discoverAllEntryPoints(module);
    if (entryPoints.empty())
        return ShaderResult::err(SlangError::MissingShaderEntryPoints);

    std::vector<::slang::IComponentType*> components;
    components.reserve(1 + entryPoints.size());
    components.emplace_back(module);
    for (const auto& entryPoint : entryPoints)
        components.emplace_back(entryPoint);

    Slang::ComPtr<::slang::IComponentType> composed;
    if (SLANG_FAILED(session->createCompositeComponentType(components.data(), static_cast<SlangInt>
        (components.size()), composed.writeRef(), diagnostics.writeRef()))) {
        return ShaderResult::err(SlangError::FailedToComposeShader);
    }

    Slang::ComPtr<::slang::IComponentType> linked;
    if (SLANG_FAILED(composed->link(linked.writeRef(), diagnostics.writeRef()))) {
        return ShaderResult::err(SlangError::FailedToLinkShader);
    }

    Slang::ComPtr<::slang::IBlob> layoutDiagnostics;
    ::slang::ProgramLayout* programLayout = linked->getLayout(0, layoutDiagnostics.writeRef());
    if (!programLayout)
        return ShaderResult::err(SlangError::FailedToGenerateEntryPointCode);

    std::vector<renderium::shader::ShaderEntryPoint> shaderEntryPoints;
    shaderEntryPoints.reserve(entryPoints.size());
    for (size_t i = 0; i < entryPoints.size(); i++) {
        ::slang::EntryPointReflection* epReflec = programLayout->getEntryPointByIndex(i);
        const auto classified = classifyStage(epReflec->getStage());
        if (!classified) continue;

        Slang::ComPtr<::slang::IBlob> blob;
        if (SLANG_FAILED(linked->getEntryPointCode(i, 0, blob.writeRef(), diagnostics.writeRef()))) {
            return ShaderResult::err(SlangError::FailedToGenerateEntryPointCode);
        }

        std::vector bytes(
            static_cast<const uint8_t*>(blob->getBufferPointer()),
            static_cast<const uint8_t*>(blob->getBufferPointer()) + blob->getBufferSize());

        shaderEntryPoints.emplace_back(renderium::shader::ShaderEntryPoint{
            *classified, epReflec->getName(), std::move(bytes)
        });
    }

    return ShaderResult::ok({compileLanguage, std::move(shaderEntryPoints)});
}

}