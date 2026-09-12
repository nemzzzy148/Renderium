//
// Created by Nemesis Verstraete on 04/09/2026.
//

#pragma once
#include <Error.h>

#include "Pipeline.h"

namespace rhi {

// render pipeline

template<typename Api>
class RenderPipelineImpl : renderium::RenderPipeline::Impl {
public:
    using Pipeline = Api::RenderPipeline;
    using PipelineResult = renderium::Result<std::unique_ptr<RenderPipelineImpl>, renderium::Error>;
    static PipelineResult create(const renderium::RenderPipelineCreateInfo& createInfo) {
        auto result = Pipeline::create(createInfo);
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