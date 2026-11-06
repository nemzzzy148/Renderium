//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "Device.h"

#include "Queue.h"
#include "Shader.h"
#include "Error.h"
#include "resource/ResourceManager.h"

namespace renderium {

ShaderModule Device::createShaderModule(const ShaderModuleDescriptor& descriptor) const {
    return ShaderModule(impl->compileShader(descriptor.code));
}

RenderPipeline Device::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
    return RenderPipeline(impl->createRenderPipeline(descriptor));
}

CommandEncoder Device::createCommandEncoder() const {
    return CommandEncoder(impl->createCommandEncoder());
}

Device::Device(std::unique_ptr<Impl> impl, std::unique_ptr<Queue::Impl> queueImpl, const SupportedFeatures features,
    const SupportedLimits &limits, const AdapterInfo &adapterInfo) : features(features), limits(limits),
    adapterInfo(adapterInfo), impl(std::move(impl)), queueImpl(std::move(queueImpl)) {}

Buffer Device::createBuffer(const BufferDescriptor &descriptor) const {
    return Buffer(impl->createBuffer(descriptor), descriptor);
}

Texture Device::createTexture(const TextureDescriptor &descriptor) const {
    return Texture(impl->createTexture(descriptor), descriptor);
}

Queue Device::queue() const {
    if (!queueImpl) // normally this error is impossible
        throw std::runtime_error("Renderium fatal error: failed to get queue on device!");
    return Queue(*queueImpl);
}

}