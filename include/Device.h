//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Queue.h"
#include "Result.h"
#include "Shader.h"
#include "ShaderCompiler.h"

namespace rhi {
template<typename Api>
class DeviceImpl;
template<typename Api>
class InstanceImpl;
template<typename Api>
class SurfaceImpl;
}

namespace renderium {

struct PipelineLayoutCreateInfo;
class PipelineLayout;
class Shader;
struct ComputePipelineCreateInfo;
class ComputePipeline;
struct RenderPipelineCreateInfo;
class RenderPipeline;
class Queue;
class Surface;

enum class PowerPreferences {
    HighPerformance,
    LowPower,
    Auto
};

struct DeviceCreateInfo {
    const Surface* compatibleSurface = nullptr;
    PowerPreferences powerPreferences = PowerPreferences::HighPerformance;
};


class Device {
public:
    Device() = delete;

    using ShaderResult = Result<Shader, Error>;
    ShaderResult createShader(const std::string& shaderCode);
    using PipelineLayoutResult = Result<PipelineLayout, Error>;
    PipelineLayoutResult createPipelineLayout(const PipelineLayoutCreateInfo& createInfo);
    using RenderPipelineResult = Result<RenderPipeline, Error>;
    RenderPipelineCreateInfo createRenderPipeline(const RenderPipelineCreateInfo& createInfo);

    [[nodiscard]] Queue getQueue() const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<shader::CompiledShader, Error> createShader(const char* compiledCode) = 0;

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
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    friend class Instance;
    friend class Queue;
};

}