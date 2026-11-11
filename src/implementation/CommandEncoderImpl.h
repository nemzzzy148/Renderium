//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <optional>
#include <memory>

#include "CommandEncoder.h"
#include "DeviceState.h"
#include "vulkan/VulkanCommandEncoder.h"
#include "Texture.h"

namespace rhi {

template<track::Usage Usage>
struct Transformation {
    resource::ResourceId id;
    std::optional<Usage> beginUsage;
    Usage endUsage;
};

enum class CommandUnitType {
    TextureTransformation,
    BufferTransformation,
    RenderPass,
    ComputePass
};

struct CommandCollection {
    std::vector<std::pair<CommandUnitType, uint32_t>> commandUnits;
    std::vector<std::byte> data;
};

template<typename Api>
class CommandEncoderImpl : public renderium::CommandEncoder::Impl {
public:
    using Device = Api::Device;
    using CommandEncoder = Api::CommandEncoder;

    std::shared_ptr<IPassImpl>
        beginRenderPass(const renderium::RenderPassDescriptor &descriptor) override;
    std::unique_ptr<renderium::CommandBuffer::Impl>
        finish(const renderium::CommandBufferDescriptor &descriptor) override;
private:
    explicit CommandEncoderImpl(std::shared_ptr<DeviceState<Api>> device, std::unique_ptr<CommandEncoder> commandEncoder)
        : deviceState(std::move(device)), commandEncoder(std::move(commandEncoder)) {}

    CommandCollection generateCommandCollection();

    template<track::Usage Usage>
    static void insertChanges(track::ScopeTracker<Usage>& scope, CommandCollection& collection, CommandUnitType type);
    template<typename Unit>
    static Unit& insertCommand(CommandCollection& collection, CommandUnitType type);

    std::shared_ptr<DeviceState<Api>> deviceState;
    std::unique_ptr<CommandEncoder> commandEncoder;

    std::queue<std::shared_ptr<IPassImpl>> passes;
    bool hasFinished = false;

    friend class DeviceImpl<Api>;
};

}
