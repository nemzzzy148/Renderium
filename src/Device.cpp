//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "Device.h"

#include "Queue.h"
#include "Shader.h"
#include "Error.h"

namespace renderium {

Device::ShaderModuleResult Device::createShaderModule(const ShaderModuleDescriptor& descriptor) const {
    auto compiledShaderResult = shaderCompiler->compileShader(descriptor.code);
    if (!compiledShaderResult.isOk()) {
        return ShaderModuleResult::err(compiledShaderResult.unwrapError());
    }
    return ShaderModuleResult::ok(ShaderModule(std::move(compiledShaderResult.unwrap())));
}

Device::RenderPipelineResult Device::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
    auto result = impl->createRenderPipeline(descriptor);
    if (!result.isOk()) {
        return RenderPipelineResult::err(result.unwrapError());
    }
    return RenderPipelineResult::ok(RenderPipeline(std::move(result.unwrap())));
}

Queue Device::getQueue() const {
    if (!queueImpl) // normally this error is impossible
        throw std::runtime_error("Renderium fatal error: failed to get queue on device!");
    return Queue(*queueImpl);
}

}