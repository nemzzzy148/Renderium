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

enum class PassType {
    Render,
    Compute
};


struct IPassImpl {
    virtual ~IPassImpl() = default;
    PassType type;
    explicit IPassImpl(const PassType type) : type(type) {}

    virtual void setPipeline(const renderium::detail::Resource& pipeline) = 0;
    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
        uint32_t firstInstance) = 0;

    virtual void end() = 0;
};

template<PassType Type, typename Api>
class PassImpl;

template<typename Api>
class PassImpl<PassType::Render, Api> : public IPassImpl {
public:
    void begin(const renderium::RenderPassDescriptor& descriptor);
    void setPipeline(const renderium::detail::Resource& pipeline) override;
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void end() override;
private:
    explicit PassImpl(std::weak_ptr<DeviceState<Api>> weakDeviceState)
        : IPassImpl(PassType::Render), weakDeviceState(std::move(weakDeviceState)) {}
    std::vector<std::byte> data;

    track::ScopeTracker<renderium::TextureUsages> textureScope;
    track::ScopeTracker<renderium::BufferUsages> bufferScope;

    template<typename Command>
    Command& insertCommand(CommandType type);

    std::queue<uint32_t> commands; // command type and offset in bytes inside data.
    bool hasEnded = false;

    std::weak_ptr<DeviceState<Api>> weakDeviceState;
};

}
