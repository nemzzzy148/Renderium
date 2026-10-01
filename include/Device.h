//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Pipeline.h"
#include "Queue.h"
#include "Result.h"
#include "Shader.h"
#include "ShaderCompiler.h"

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

enum class PowerPreference {
    None,
    HighPerformance,
    LowPower
};

struct DeviceDescriptor {

};


class Device {
public:
    Device() = delete;

    using ShaderModuleResult = Result<ShaderModule, Error>;
    [[nodiscard]] ShaderModuleResult createShaderModule(const ShaderModuleDescriptor& descriptor) const;
    using PipelineLayoutResult = Result<PipelineLayout, Error>;
    PipelineLayoutResult createPipelineLayout(const PipelineLayoutDescriptor& descriptor);
    using RenderPipelineResult = Result<RenderPipeline, Error>;
    [[nodiscard]] RenderPipelineResult createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

    [[nodiscard]] Queue getQueue() const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<RenderPipeline::Impl>, Error>
            createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;

        virtual Result<std::unique_ptr<Queue::Impl>, Error> createQueue() = 0;
    };
    explicit Device(std::unique_ptr<Impl> impl, std::unique_ptr<Queue::Impl> queueImpl,
        std::unique_ptr<shader::ShaderCompiler> shaderCompiler) : impl(std::move(impl)),
                                                                  queueImpl(std::move(queueImpl)),
                                                                  shaderCompiler(std::move(shaderCompiler)) {}

    std::unique_ptr<Impl> impl;
    std::unique_ptr<Queue::Impl> queueImpl;
    std::unique_ptr<shader::ShaderCompiler> shaderCompiler;

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