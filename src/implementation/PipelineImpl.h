//
// Created by Nemesis Verstraete on 04/09/2026.
//

#pragma once
#include <algorithm>
#include <Error.h>

namespace rhi {

inline const renderium::shader::ShaderEntryPoint* findEntryPoint(const std::vector<renderium::shader::ShaderEntryPoint>& entryPoints,
                                                                 const std::string& functionName) {
    const auto it = std::ranges::find_if(entryPoints, [functionName](const renderium::shader::ShaderEntryPoint& eP) {
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

    [[nodiscard]] bool isValid() const { return pipeline.has_value(); }
private:
    explicit RenderPipelineImpl() : pipeline(std::nullopt) {}
    explicit RenderPipelineImpl(Pipeline pipeline) : pipeline(std::move(pipeline)) {}
    std::optional<Pipeline> pipeline;

    friend class DeviceImpl<Api>;
};

// compute pipeline

}