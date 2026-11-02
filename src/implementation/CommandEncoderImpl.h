//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <optional>
#include <memory>

#include "CommandEncoder.h"

namespace rhi {

template<typename Api>
class CommandEncoderImpl : public renderium::CommandEncoder::Impl {
public:
    using CommandEncoder = Api::CommandEncoder;
private:
    explicit CommandEncoderImpl(std::shared_ptr<CommandEncoder> commandEncoder)
        : commandEncoder(std::move(commandEncoder)) {}
    explicit CommandEncoderImpl() : commandEncoder(std::nullopt) {}
    std::optional<std::shared_ptr<CommandEncoder> > commandEncoder;

    friend class DeviceImpl<Api>;
};

}
