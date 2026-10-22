//
// Created by Nemesis Verstraete on 04/09/2026.
//

#pragma once
#include <algorithm>
#include <optional>
#include <Error.h>

#include "Pipeline.h"
#include "Shader.h"

namespace rhi {
class DeviceErrorState;

const renderium::shader::ShaderEntryPoint* findEntryPoint(
    const std::vector<renderium::shader::ShaderEntryPoint>& entryPoints, const std::string& functionName);

bool validateRenderPipelineDescriptor(DeviceErrorState& devicErrorState,
    const renderium::RenderPipelineDescriptor& descriptor);

// render pipeline

template<typename Api>
class RenderPipelineImpl : public renderium::RenderPipeline::Impl {
public:
    using Pipeline = Api::RenderPipeline;

    [[nodiscard]] bool isValid() const { return pipeline.has_value(); }
private:
    explicit RenderPipelineImpl() : pipeline(std::nullopt) {}
    explicit RenderPipelineImpl(Pipeline pipeline) : pipeline(std::move(pipeline)) {}
    std::optional<Pipeline> pipeline;

    friend class DeviceImpl<Api>;
};

// compute pipeline

}
