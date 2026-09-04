//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Queue.h"
#include "Result.h"

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

struct ShaderCreateInfo {
    const char* code;
};

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

    using ShaderResult = Result<ComputePipeline, Error>;
    ShaderResult createShader(const ShaderCreateInfo& createInfo);
    using PipelineLayoutResult = Result<PipelineLayout, Error>;
    PipelineLayoutResult createPipelineLayout(const PipelineLayoutCreateInfo& createInfo);
    using RenderPipelineResult = Result<RenderPipeline, Error>;
    RenderPipelineCreateInfo createRenderPipeline(const RenderPipelineCreateInfo& createInfo);

    [[nodiscard]] Queue getQueue() const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<Queue::Impl>, Error> createQueue() = 0;
    };
    explicit Device(std::unique_ptr<Impl> impl, std::unique_ptr<Queue::Impl> queueImpl)
        : impl(std::move(impl)), queueImpl(std::move(queueImpl)) {}
    std::unique_ptr<Impl> impl;
    std::unique_ptr<Queue::Impl> queueImpl;

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