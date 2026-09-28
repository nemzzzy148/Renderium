//
// Created by Nemesis Verstraete on 04/09/2026.
//

#pragma once
#include <algorithm>
#include <Error.h>
#include <optional>

#include "Pipeline.h"

namespace rhi {

inline const renderium::shader::ShaderEntryPoint* findEntryPoint(const std::vector<renderium::shader::ShaderEntryPoint>& entryPoints,
                                                                 const std::string& functionName) {
    auto it = std::ranges::find_if(entryPoints, [functionName](const renderium::shader::ShaderEntryPoint& eP) {
        return eP.name == functionName;
    });
    if (it == entryPoints.end()) return nullptr;
    return &*it;
}

// render pipeline

template<typename Api>
class RenderPipelineImpl : renderium::RenderPipeline::Impl {
public:
    using Pipeline = Api::RenderPipeline;
    using PipelineResult = renderium::Result<std::unique_ptr<RenderPipelineImpl>, renderium::Error>;
    static PipelineResult create(const Api::Device& device, const renderium::RenderPipelineCreateInfo& createInfo) {
        auto result = Pipeline::create(device, createInfo);
        if (!result.isOk()) {
            return PipelineResult::err(renderium::Error::RenderPipelineCreateError);
        }
        return PipelineResult::ok(std::unique_ptr<RenderPipelineImpl>(
            new RenderPipelineImpl(std::move(result.unwrap()))));
    }
private:
    explicit RenderPipelineImpl(Pipeline pipeline) : pipeline(std::move(pipeline)) {}
    Pipeline pipeline;
};

// compute pipeline

}