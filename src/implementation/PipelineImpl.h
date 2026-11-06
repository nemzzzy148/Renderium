//
// Created by Nemesis Verstraete on 04/09/2026.
//

#pragma once
#include <algorithm>
#include <optional>

#include "DeviceImpl.h"
#include "IPipeline.h"
#include "Pipeline.h"
#include "Resource.h"
#include "DeviceState.h"

namespace rhi {
class Process;

const renderium::shader::ShaderEntryPoint* findEntryPoint(
    const std::vector<renderium::shader::ShaderEntryPoint>& entryPoints, const std::string& functionName);

template<typename Api>
class RenderPipelineResource final : public renderium::detail::Resource {
public:
    RenderPipelineResource(std::shared_ptr<DeviceState<Api>> deviceState, const resource::ResourceId id)
        : id(id), deviceState(std::move(deviceState)) {}

    ~RenderPipelineResource() override {
        deviceState->renderPipelines.removeRef(id.handle, id.version);
    }
private:
    resource::ResourceId id;
    std::shared_ptr<DeviceState<Api>> deviceState;
};

void validateRenderPipelineDescriptor(Process& process, const renderium::RenderPipelineDescriptor &descriptor);

// render pipeline

template<typename Api>
class RenderPipelineImpl : public IRenderPipeline {
public:
    using Pipeline = Api::RenderPipeline;
private:
    explicit RenderPipelineImpl(Pipeline pipeline) : pipeline(std::move(pipeline)) {}
    Pipeline pipeline;

    friend class DeviceImpl<Api>;
};

// compute pipeline

}
