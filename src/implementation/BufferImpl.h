//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include <algorithm>

#include "Buffer.h"

namespace rhi {

template<typename Api>
class BufferImpl : public renderium::Buffer::Impl {
public:
    using Buffer = Api::Buffer;

    [[nodiscard]] bool isValid() const { return buffer.has_value(); }
private:
    explicit BufferImpl(Buffer buffer, const renderium::BufferDescriptor& descriptor)
        : Impl(descriptor.size, descriptor.usages, descriptor.mappedAtCreation ?
            renderium::BufferMapState::Mapped : renderium::BufferMapState::Unmapped),
          buffer(std::move(buffer)) {}
    explicit BufferImpl()
        : Impl(0, renderium::BufferUsages::None, renderium::BufferMapState::Unmapped), buffer(std::nullopt) {}

    std::optional<Buffer> buffer;

    friend class DeviceImpl<Api>;
};

}
