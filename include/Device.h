//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "AdapterInfo.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "Queue.h"
#include "Result.h"
#include "Shader.h"
#include "Texture.h"

namespace rhi {
template<typename Api>
class DeviceImpl;
template<typename Api>
class AdapterImpl;
template<typename Api>
class SurfaceImpl;
}

namespace renderium {

struct PipelineLayoutDescriptor;
class PipelineLayout;
class ShaderModule;
struct ComputePipelineDescriptor;
class ComputePipeline;
struct RenderPipelineDescriptor;
class RenderPipeline;
class Queue;
class Surface;

struct DeviceDescriptor {
    ShadingLanguage shadingLanguage = ShadingLanguage::SLANG;
};

class Device {
public:
    Device() = delete;

    const SupportedFeatures features;
    const SupportedLimits limits;
    const AdapterInfo adapterInfo;

    [[nodiscard]] Queue queue() const;

    [[nodiscard]] Buffer createBuffer(const BufferDescriptor& descriptor) const;
    [[nodiscard]] Texture createTexture(const TextureDescriptor& descriptor) const;

    [[nodiscard]] ShaderModule createShaderModule(const ShaderModuleDescriptor& descriptor) const;
    // PipelineLayout createPipelineLayout(const PipelineLayoutDescriptor& descriptor);
    [[nodiscard]] RenderPipeline createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<Queue::Impl>, RequestError> createQueue() = 0;

        virtual std::unique_ptr<Buffer::Impl> createBuffer(const BufferDescriptor& descriptor) = 0;
        virtual std::unique_ptr<Texture::Impl> createTexture(const TextureDescriptor& descriptor) = 0;

        virtual shader::CompiledShader compileShader(const std::string& code) = 0;
        virtual std::unique_ptr<RenderPipeline::Impl>
            createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;
    };
    explicit Device(std::unique_ptr<Impl> impl, std::unique_ptr<Queue::Impl> queueImpl,
        const SupportedFeatures features, const SupportedLimits &limits, const AdapterInfo& adapterInfo) :
        features(features), limits(limits), adapterInfo(adapterInfo),
        impl(std::move(impl)), queueImpl(std::move(queueImpl)) {}

    std::unique_ptr<Impl> impl;
    std::unique_ptr<Queue::Impl> queueImpl;

    template<typename Api>
    friend class rhi::DeviceImpl;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    template<typename Api>
    friend class rhi::AdapterImpl;
    friend class Queue;
    friend class Adapter;
};

}

template<> struct utils::enumBitwiseOperations<renderium::SupportedFeatures> : std::true_type {};