//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include <algorithm>
#include <optional>

#include "Buffer.h"
#include "DeviceImpl.h"
#include "IBuffer.h"
#include "Resource.h"
#include "DeviceState.h"

namespace rhi {
class Process;

template<typename Api>
class BufferResource final : public renderium::detail::Resource {
public:
    BufferResource(std::shared_ptr<DeviceState<Api>> deviceState, const resource::ResourceId id)
        : id(id), deviceState(std::move(deviceState)) {}

    ~BufferResource() override {
        deviceState->buffers.removeRef(id.handle, id.version);
    }
private:
    resource::ResourceId id;
    std::shared_ptr<DeviceState<Api>> deviceState;
};

void validateBufferDescriptor(Process& process, const renderium::BufferDescriptor &descriptor);

template<typename Api>
class BufferImpl : public IBuffer {
public:
    using Buffer = Api::Buffer;
private:
    explicit BufferImpl(Buffer buffer)
        : buffer(std::move(buffer)) {}

    Buffer buffer;

    friend class DeviceImpl<Api>;
};

}
