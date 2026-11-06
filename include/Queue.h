//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

namespace rhi {
template<typename Api>
class QueueImpl;
template<typename Api>
class InstanceImpl;
template<typename Api>
class DeviceImpl;
}

namespace renderium {
class Device;
enum class RequestError;
class CommandBuffer;

class Queue {
public:
private:
    struct Impl {
        ~Impl() = default;
    };
    std::shared_ptr<Impl> impl;

    explicit Queue(std::shared_ptr<Impl> impl) : impl(std::move(impl)) {}
    template<typename Api>
    friend class rhi::QueueImpl;
    template<typename Api>
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
    friend class Instance;
    friend class Device;
};

}
