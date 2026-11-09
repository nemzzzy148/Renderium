//
// Created by Nemesis Verstraete on 09/11/2026.
//

#include "PassImpl.h"

#include "PipelineImpl.h"
#include "Texture.h"

namespace rhi {
template<typename Api>

void RenderPassImpl<Api>::begin(const renderium::RenderPassDescriptor &descriptor) {
    auto deviceState = weakDeviceState.lock();
    if (!deviceState) return; // validation(device state is invalid)

    for (auto& attachment : descriptor.colorAttachments) {
        if (!attachment.textureView.textureResource) continue; // validation(view was created with a dead texture)
        auto resource = static_cast<const TextureResource<Api>&>(*attachment.textureView.textureResource);
        if (&*resource.deviceState != &*deviceState) continue; // validation(devices don't match)
    }
}

template<typename Api>
void RenderPassImpl<Api>::setPipeline(const renderium::detail::Resource &pipeline) {
    auto device = weakDeviceState.lock();
    if (!device) return;

    const auto& pipelineResource = static_cast<const RenderPipelineResource<Api>&>(pipeline);
    if (&*device != &*pipelineResource.deviceState) return; // not the same device

    auto& [id] = insertCommand<SetPipelineCommand>(CommandType::SetPipeline);
    id = pipelineResource.id;
}

template<typename Api>
void RenderPassImpl<Api>::draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                          const uint32_t firstInstance) {
    auto& command = insertCommand<DrawCommand>(CommandType::Draw);
    command.vertexCount = vertexCount;
    command.instanceCount = instanceCount;
    command.firstVertex = firstVertex;
    command.firstInstance = firstInstance;
}

template<typename Api>
template<typename Command>
Command& RenderPassImpl<Api>::insertCommand(const CommandType type) {
    const uint32_t offset = data.size();
    data.resize(offset + sizeof(CommandType) + sizeof(Command));
    std::memcpy(&data[offset], &type, sizeof(CommandType));

    commands.push(offset);

    return *new (&data[offset + sizeof(CommandType)])Command{};
}

}