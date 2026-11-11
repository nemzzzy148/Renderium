//
// Created by Nemesis Verstraete on 11/11/2026.
//

#include "CommandEncoderImpl.h"

#include "PassImpl.h"

namespace rhi {

template<typename Api>
std::shared_ptr<IPassImpl> CommandEncoderImpl<Api>::beginRenderPass(
    const renderium::RenderPassDescriptor &descriptor) {
    if (hasFinished) return nullptr;
    passes.emplace(std::make_shared<IPassImpl>(new RenderPassImpl<Api>(deviceState)));
    return passes.back();
}

template<typename Api>
std::unique_ptr<renderium::CommandBuffer::Impl> CommandEncoderImpl<Api>::finish(
    const renderium::CommandBufferDescriptor &descriptor) {
    if (hasFinished) return nullptr;
    hasFinished = true;

    auto commandCollection = generateCommandCollection(); // queue is invalid after this

    return std::unique_ptr<renderium::CommandBuffer::Impl>(
        new CommandBufferImpl<Api>(std::move(deviceState), std::move(commandEncoder), std::move(renderPasses)));
}

template<typename Api>
CommandCollection CommandEncoderImpl<Api>::generateCommandCollection() {
    CommandCollection collection;
    collection.commandUnits.reserve(3 * passes.size()); // average amount of transformations is 2 -> 2 + 1
    // Average size of the data buffer
    collection.data.reserve((sizeof(void *) + sizeof(Transformation<renderium::TextureUsages>) * 2) * passes.size());

    while (!passes.empty()) {
        std::shared_ptr<IPassImpl> pass = std::move(passes.front());
        passes.pop();

        if (pass->type == PassType::Render) {
            const auto& renderPass = static_cast<const PassImpl<PassType::Render, Api>&>(*pass);
            insertChanges(renderPass.bufferScope, collection, CommandUnitType::BufferTransformation);
            insertChanges(renderPass.textureScope, collection, CommandUnitType::TextureTransformation);
            auto& command = insertCommand<std::shared_ptr<IPassImpl>>
                (collection, CommandUnitType::RenderPass);
            command = std::move(pass);
        }
        // implement compute
    }

    return std::move(collection);
}

template<typename Api>
template<track::Usage Usage>
void CommandEncoderImpl<Api>::insertChanges(track::ScopeTracker<Usage> &scope, CommandCollection &collection, const CommandUnitType type) {
    for (auto& change : scope.getChanges()) {
        if (!change.second.begin.has_value() && change.second.begin == change.second.end) continue;
        auto& command = insertCommand<Transformation<renderium::BufferUsages>>(collection, type);
        command.id = change.first;
        command.beginUsage = change.second.begin;
        command.endUsage = change.second.end;
    }
}

template<typename Api>
template<typename Unit>
Unit& CommandEncoderImpl<Api>::insertCommand(CommandCollection& collection, const CommandUnitType type) {
    const uint32_t size = collection.data.size();
    collection.data.resize(size + sizeof(Unit) + sizeof(CommandUnitType));
    std::memcpy(&collection.data[size], &type, sizeof(CommandUnitType));

    collection.commandUnits.emplace_back(type, size);

    return * new(&collection.data[size + sizeof(CommandUnitType)])Unit{};
}

}
