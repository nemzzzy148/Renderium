//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <memory>

namespace rhi {
template<typename Api>
class CommandEncoderImpl;
template<typename Api>
class DeviceImpl;
}

namespace renderium {

class CommandEncoder {
    struct Impl {
        virtual ~Impl() = default;
    };
    std::unique_ptr<Impl> impl;
    explicit CommandEncoder(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}

    friend class Device;

    template<typename Api>
    friend class rhi::CommandEncoderImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}
