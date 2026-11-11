//
// Created by Nemesis Verstraete on 02/11/2026.
//

#pragma once
#include <memory>

#include "CommandBuffer.h"
#include "Pass.h"

namespace rhi {
template<typename Api>
class CommandEncoderImpl;
template<typename Api>
class DeviceImpl;
}

namespace renderium {
struct CommandBufferDescriptor;
struct RenderPassDescriptor;
class RenderPass;
class CommandBuffer;

class CommandEncoder {
public:
    [[nodiscard]] RenderPass beginRenderPass(const RenderPassDescriptor& descriptor) const;

    [[nodiscard]] CommandBuffer finish() const;
    [[nodiscard]] CommandBuffer finish(const CommandBufferDescriptor& descriptor) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual std::shared_ptr<rhi::IPassImpl> beginRenderPass(const RenderPassDescriptor& descriptor) = 0;
        virtual std::unique_ptr<CommandBuffer::Impl> finish(const CommandBufferDescriptor& descriptor) = 0;
    };
    std::unique_ptr<Impl> impl; // can be nullptr
    explicit CommandEncoder(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}

    friend class Device;

    template<typename Api>
    friend class rhi::CommandEncoderImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}
