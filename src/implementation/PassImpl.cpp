//
// Created by Nemesis Verstraete on 09/11/2026.
//

#include "PassImpl.h"

#include "PipelineImpl.h"
#include "Texture.h"

namespace rhi {

template<typename Api>
void PassImpl<PassType::Render, Api>::begin(const renderium::RenderPassDescriptor &descriptor) {
    auto deviceState = weakDeviceState.lock();
    if (!deviceState) return; // validation(device state is invalid)

    for (auto& attachment : descriptor.colorAttachments) {
        if (!attachment.textureView.textureResource) continue; // validation(view was created with a dead texture)
        auto resource = static_cast<const TextureResource<Api>&>(*attachment.textureView.textureResource);
        if (&*resource.deviceState != &*deviceState) continue; // validation(devices don't match)

        const resource::ResourceId id = resource.id;
        if (textureScope.hasUsage(id)) continue; // already has attachment
        textureScope.addUsage(id,
            resource.deviceState->textureTracker.get(id), renderium::TextureUsages::RenderAttachment);
    }
}

template<typename Api>
void PassImpl<PassType::Render, Api>::setPipeline(const renderium::detail::Resource &pipeline) {
    if (hasEnded) return;
    auto device = weakDeviceState.lock();
    if (!device) return;

    const auto& pipelineResource = static_cast<const RenderPipelineResource<Api>&>(pipeline);
    if (&*device != &*pipelineResource.deviceState) return; // not the same device

    auto& [id] = insertCommand<SetPipelineCommand>(CommandType::SetPipeline);
    id = pipelineResource.id;
}

template<typename Api>
void PassImpl<PassType::Render, Api>::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
    uint32_t firstInstance) {
    if (hasEnded) return;
    auto& command = insertCommand<DrawCommand>(CommandType::Draw);
    command.vertexCount = vertexCount;
    command.instanceCount = instanceCount;
    command.firstVertex = firstVertex;
    command.firstInstance = firstInstance;
}

template<typename Api>
void PassImpl<PassType::Render, Api>::end() {
    hasEnded = true;
}

template<typename Api>
template<typename Command>
Command & PassImpl<PassType::Render, Api>::insertCommand(CommandType type) {
    const uint32_t offset = data.size();
    data.resize(offset + sizeof(CommandType) + sizeof(Command));
    std::memcpy(&data[offset], &type, sizeof(CommandType));

    commands.push(offset);

    return *new (&data[offset + sizeof(CommandType)])Command{};
}

}