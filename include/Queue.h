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
enum class Error;
class CommandBuffer;

class Queue {
public:
private:
    struct Impl {

    };
    const Impl& impl;

    explicit Queue(const Impl& impl) : impl(impl) {}
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
