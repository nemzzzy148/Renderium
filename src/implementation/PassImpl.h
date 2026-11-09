//
// Created by Nemesis Verstraete on 03/11/2026.
//

#pragma once
#include <queue>

#include "DeviceState.h"
#include "Pass.h"
#include "../track/ScopeTracker.h"

namespace rhi {

enum class CommandType {
    SetPipeline,
    Draw
};

struct SetPipelineCommand {
    resource::ResourceId id;
};

struct DrawCommand {
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstVertex = 0;
    uint32_t firstInstance = 0;
};

template<typename Api>
class RenderPassImpl : public renderium::RenderPass::Impl {
public:
    void begin(const renderium::RenderPassDescriptor& descriptor);
    void setPipeline(const renderium::detail::Resource& pipeline) override;
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
private:
    explicit RenderPassImpl(std::weak_ptr<DeviceState<Api>> weakDeviceState)
        : weakDeviceState(std::move(weakDeviceState)) {}
    std::vector<std::byte> data;

    track::ScopeTracker<renderium::TextureUsages> textureScope;
    track::ScopeTracker<renderium::BufferUsages> bufferScope;

    template<typename Command>
    Command& insertCommand(CommandType type);

    std::queue<uint32_t> commands; // command type and offset in bytes inside data.

    std::weak_ptr<DeviceState<Api>> weakDeviceState;
};

}
