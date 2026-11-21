//
// Created by Nemesis Verstraete on 14/11/2026.
//

#include "CommandTranslation.h"

#include "PassImpl.h"
#include "PipelineImpl.h"

namespace rhi {

template<typename Api>
std::optional<renderium::RequestError> CommandTranslation<Api>::translateCommands(CommandBufferImpl<Api> &commandBuffer) {
    auto& data = commandBuffer.collection.data;

    commandBuffer.begin();
    for (auto& command : commandBuffer.collection.commandUnits) {
        switch (command.first) {
            case CommandUnitType::TextureTransformation: {
                auto& transformation =
                    dataToObject<Transformation<renderium::TextureUsages>>(data, command.second);
                translateTexture(*commandBuffer.commandBuffer, transformation);
                break;
            }
            case CommandUnitType::BufferTransformation: {
                break;
            }
            case CommandUnitType::ComputePass: {
                break;
            }
            case CommandUnitType::RenderPass: {
                auto& pass = dataToObject<std::shared_ptr<IPassImpl>>(data, command.second);
                translateRenderPass(commandBuffer, pass);
                break;
            }
            default: break; // gift for compiler
        }
    }
    commandBuffer.end();

    return std::nullopt;
}

template<typename Api>
void CommandTranslation<Api>::translateRenderPass(CommandBufferImpl<Api> &commandBuffer,
    std::shared_ptr<IPassImpl>& pass) {
    auto& renderPass = static_cast<PassImpl<PassType::Render, Api>>(*pass);
    auto& nativeCB = commandBuffer.getNativeCommandBuffer();
    auto& deviceState = *commandBuffer.deviceState;

    while (!renderPass.commands.empty()) {
        const auto& command = renderPass.commands.front();
        const uint32_t& offset = command.second;

        switch (command.first) {
            case CommandType::Draw: {
                auto& [vertexCount, instanceCount, firstVertex, firstInstance] =
                    dataToObject<DrawCommand&>(renderPass.data, offset);
                nativeCB.draw(vertexCount, instanceCount, firstVertex, firstInstance);
                break;
            }
            case CommandType::SetPipeline: {
                auto& pipelineCommand = dataToObject<SetPipelineCommand&>(renderPass.data, offset);
                auto* pipeline = static_cast<RenderPipelineImpl<Api>*>(deviceState.renderPipelines.get(pipelineCommand.id));
                if (!pipeline) break;

                nativeCB.setPipeline(pipeline->pipeline);
                break;

            }
            default: break; // gift for compiler
        }

        renderPass.commands.pop();
    }
}

template<typename Api>
template<typename T>
T& CommandTranslation<Api>::dataToObject(const std::vector<std::byte> &data, const uint32_t offset) {
    return *new(&data[offset])T{};
}

}
