//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "PipelineImpl.h"
#include "DeviceErrorState.h"

namespace rhi {

const renderium::shader::ShaderEntryPoint* findEntryPoint(
    const std::vector<renderium::shader::ShaderEntryPoint> &entryPoints, const std::string &functionName) {

    const auto it = std::ranges::find_if(entryPoints, [functionName]
            (const renderium::shader::ShaderEntryPoint& eP) {
        return eP.name == functionName;
    });
    if (it == entryPoints.end()) return nullptr;
    return &*it;
}

void validateRenderPipelineDescriptor(Process& process, const renderium::RenderPipelineDescriptor &descriptor) {
    if (!descriptor.vertex.module.isValid()) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The vertex shader module is not valid for creating a render pipeline."
        });
    }

    if (descriptor.vertex.entryPoint.empty()) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The vertex entry point string is empty. "
            "The string content is required to locate the entry point function for the vertex state."
        });
    }

    if (!descriptor.fragment.module.isValid()) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The fragment shader module is not valid for creating a render pipeline."
        });
    }

    if (descriptor.fragment.entryPoint.empty()) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The fragment entry point string is empty. "
            "The string content is required to locate the entry point function for the fragment state."
        });
    }

    if (descriptor.fragment.targets.empty()) {
        process.report({
            renderium::ErrorFilter::Validation,
            "No render targets were specified when creating a render pipeline."
        });
    }
}

}