//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <future>
#include <memory>
#include <optional>

#include "AdapterInfo.h"
#include "Buffer.h"
#include "CommandEncoder.h"
#include "Pipeline.h"
#include "Queue.h"
#include "Result.h"
#include "Shader.h"
#include "Texture.h"
#include "Error.h"

namespace rhi {
template<typename Api>
class DeviceImpl;
template<typename Api>
class AdapterImpl;
template<typename Api>
class SurfaceImpl;
}

namespace renderium {
class CommandEncoder;

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

    void pushErrorScope(const ErrorFilter filter) const { impl->pushErrorScope(filter); }
    [[nodiscard]] std::future<std::optional<Error>> popErrorScope() const { return impl->popErrorScope(); }

    [[nodiscard]] Buffer createBuffer(const BufferDescriptor& descriptor) const;
    [[nodiscard]] Texture createTexture(const TextureDescriptor& descriptor) const;

    [[nodiscard]] ShaderModule createShaderModule(const ShaderModuleDescriptor& descriptor) const;
    // PipelineLayout createPipelineLayout(const PipelineLayoutDescriptor& descriptor);
    [[nodiscard]] RenderPipeline createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

    [[nodiscard]] CommandEncoder createCommandEncoder() const;
private:
    struct Impl {
        virtual ~Impl() = default;

        using ResourceResult = std::shared_ptr<detail::Resource>;

        virtual Result<std::shared_ptr<Queue::Impl>, RequestError> createQueue() = 0;

        virtual void pushErrorScope(ErrorFilter filter) = 0;
        virtual std::future<std::optional<Error>> popErrorScope() = 0;

        virtual ResourceResult createBuffer(const BufferDescriptor& descriptor) = 0;
        virtual ResourceResult createTexture(const TextureDescriptor& descriptor) = 0;

        virtual shader::CompiledShader compileShader(const std::string& code) = 0;
        virtual ResourceResult createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;

        virtual std::unique_ptr<CommandEncoder::Impl> createCommandEncoder() = 0;
    };
    explicit Device(std::unique_ptr<Impl> impl, std::shared_ptr<Queue::Impl> queueImpl,
        const SupportedFeatures features, const SupportedLimits &limits, const AdapterInfo& adapterInfo);

    std::unique_ptr<Impl> impl;
    std::shared_ptr<Queue::Impl> queueImpl;

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