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

    ~CommandEncoderImpl() override {
        if (commandEncoder) {
            (*commandEncoder)->release();
        }
    }

private:
    explicit CommandEncoderImpl(std::shared_ptr<Device> device, std::shared_ptr<CommandEncoder> commandEncoder)
        : device(std::move(device)), commandEncoder(std::move(commandEncoder)) {}
    explicit CommandEncoderImpl() : device(nullptr), commandEncoder(std::nullopt) {}

    std::shared_ptr<Device> device;
    std::optional<std::shared_ptr<CommandEncoder> > commandEncoder;

    friend class DeviceImpl<Api>;
};

}
