//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

namespace rhi {
template<typename Api>
class CommandBufferImpl;
}

namespace renderium {

class CommandBuffer {
    struct Impl {
        virtual ~Impl() = default;
    };
    explicit CommandBuffer(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::CommandBufferImpl;
};

}