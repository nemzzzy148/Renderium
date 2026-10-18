//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "Device.h"

#include "Queue.h"
#include "Shader.h"
#include "Error.h"

namespace renderium {

ShaderModule Device::createShaderModule(const ShaderModuleDescriptor& descriptor) const {
    return ShaderModule(shaderCompiler->compileShader(descriptor.code));
}

RenderPipeline Device::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
    // error handling happens on the 2nd layer(implementation layer)
    return RenderPipeline(impl->createRenderPipeline(descriptor));
}

Queue Device::getQueue() const {
    if (!queueImpl) // normally this error is impossible
        throw std::runtime_error("Renderium fatal error: failed to get queue on device!");
    return Queue(*queueImpl);
}

}