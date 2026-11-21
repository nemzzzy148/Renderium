//
// Created by Nemesis Verstraete on 14/11/2026.
//

#pragma once
#include <cstdint>
#include <optional>

#include "CommandBufferImpl.h"
#include "DeviceState.h"
#include "Result.h"

namespace renderium {
enum class RequestError;
}

namespace rhi {

template<typename Api>
class CommandTranslation {
public:
    static std::optional<renderium::RequestError> translateCommands(CommandBufferImpl<Api>& commandBuffer);

    static void translateTexture(CommandBufferImpl<Api>& commandBuffer,
        Transformation<renderium::TextureUsages>& transformation);
    static void translateRenderPass(CommandBufferImpl<Api>& commandBuffer,
        std::shared_ptr<IPassImpl>& pass);
private:
    template<typename T>
    static T& dataToObject(const std::vector<std::byte>& data, uint32_t offset);
};

}
