//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

namespace rhi {
template<typename Api>
class CommandBufferImpl;
template<typename Api>
class CommandEncoderImpl;
}

namespace renderium {

struct CommandBufferDescriptor {
    // Nothing in here...
};

class CommandBuffer {
    struct Impl {
        virtual ~Impl() = default;
    };
    explicit CommandBuffer(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    friend class CommandEncoder;
    template<typename Api>
    friend class rhi::CommandBufferImpl;
    template<typename Api>
    friend class rhi::CommandEncoderImpl;
};

}