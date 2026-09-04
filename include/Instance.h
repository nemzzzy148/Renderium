//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Backend.h"
#include "Device.h"
#include "Result.h"
#include "Surface.h"

namespace rhi {
template<typename Api>
class InstanceImpl;
}

namespace renderium {
enum class Error;
class Window;

struct InstanceCreateInfo {
    Backend backend = Backend::Vulkan;
    bool debug = false;
};

class Instance {
public:
    using InstanceResult = Result<Instance, Error>;
    static InstanceResult create(const InstanceCreateInfo& createInfo);

    using SurfaceResult = Result<Surface, Error>;
    [[nodiscard]] SurfaceResult createSurface(const Window& window) const;

    using DeviceResult = Result<Device, Error>;
    [[nodiscard]] DeviceResult createDevice(const DeviceCreateInfo& createInfo) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<Surface::Impl>, Error> createSurface(const Window& window) = 0;
        virtual Result<std::unique_ptr<Device::Impl>, Error> createDevice(const DeviceCreateInfo& createInfo) = 0;
    };

    explicit Instance(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::InstanceImpl;
};

}
