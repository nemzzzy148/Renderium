//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <optional>
#include <memory>

#include "CommandEncoder.h"
#include "vulkan/VulkanCommandEncoder.h"

namespace rhi {

template<typename Api>
class CommandEncoderImpl : public renderium::CommandEncoder::Impl {
public:
    using Device = Api::Device;
    using CommandEncoder = Api::CommandEncoder;
private:
    explicit CommandEncoderImpl(std::shared_ptr<DeviceState<Api>> device, std::unique_ptr<CommandEncoder> commandEncoder)
        : deviceState(std::move(device)), commandEncoder(std::move(commandEncoder)) {}

    std::shared_ptr<DeviceState<Api>> deviceState;
    std::unique_ptr<CommandEncoder> commandEncoder;

    friend class DeviceImpl<Api>;
};

}
