//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once

#include <memory>
#include <optional>

#include "AdapterInfo.h"
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

enum class PowerPreference {
    None,
    HighPerformance,
    LowPower
};

struct RequestAdapterOptions {
    const Surface* compatibleSurface = nullptr;
    PowerPreference powerPreference = PowerPreference::None;
};

class Adapter {
public:
    const SupportedFeatures features;
    const SupportedLimits limits;
    const AdapterInfo info;

    using DeviceResult = Result<Device, RequestError>;
    [[nodiscard]] DeviceResult requestDevice(const DeviceDescriptor& descriptor) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual SupportedFeatures requestFeatures() = 0;
        virtual SupportedLimits requestLimits() = 0;
        virtual AdapterInfo requestAdapterInfo() = 0;

        virtual Result<std::unique_ptr<Device::Impl>, RequestError> requestDevice(const DeviceDescriptor& descriptor) = 0;
    };
    std::unique_ptr<Impl> impl;
    explicit Adapter(std::unique_ptr<Impl> impl, const SupportedFeatures features, const SupportedLimits &limits,
        AdapterInfo info) : features(features), limits(limits), info(std::move(info)), impl(std::move(impl)) {}

    friend class Instance;
    template<typename Api>
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::AdapterImpl;
};

}