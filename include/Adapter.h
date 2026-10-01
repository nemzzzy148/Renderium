//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once

#include <memory>
#include <optional>

#include "Device.h"
#include "Error.h"
#include "Result.h"

namespace rhi {
template<typename Api>
class InstanceImpl;
template<typename Api>
class AdapterImpl;
}

namespace renderium {
class Surface;

struct RequestAdapterOptions {
    const Surface* compatibleSurface = nullptr;
    PowerPreference powerPreference;
};

class Adapter {
public:
    using DeviceResult = Result<Device, Error>;
    [[nodiscard]] DeviceResult requestDevice(const DeviceDescriptor& descriptor) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<Device::Impl>, Error> requestDevice(const DeviceDescriptor& descriptor) = 0;
    };
    std::unique_ptr<Impl> impl;
    Backend backend;
    explicit Adapter(std::unique_ptr<Impl> impl, const Backend backend)
        : impl(std::move(impl)), backend(backend) {}

    friend class Instance;
    template<typename Api>
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::AdapterImpl;
};

}
