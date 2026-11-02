//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once
#include <cstdint>

#include "DeviceImpl.h"
#include "Surface.h"

namespace rhi {
class Process;
template<typename Api>
class InstanceImpl;

void validateSurfaceConfiguration(Process& process, const renderium::SurfaceConfiguration& configuration);

template<typename Api>
struct SurfaceImplConfiguration {
    const Api::Device& device;
    uint32_t width;
    uint32_t height;

    renderium::TextureUsages usage;
    renderium::TextureFormat format;

    renderium::PresentMode presentMode;
    uint32_t maxFramesInFlight = 3;
};

template<typename Api>
class SurfaceImpl : public renderium::Surface::Impl {
public:
    using Surface = Api::Surface;

    renderium::SurfaceCapabilities getCapabilities(const renderium::Device& device) override {
        const auto& deviceImpl = static_cast<const DeviceImpl<Api>&>(*device.impl);
        return surface.getCapabilities(deviceImpl.getBackendDevice());
    }

    void configure(const renderium::SurfaceConfiguration& configuration) override {
        auto& device = static_cast<DeviceImpl<Api>&>(*configuration.device.impl);
        auto process = Process(device.getDeviceErrorState());
        validateSurfaceConfiguration(process, configuration);

        if (process.caughtError()) return;

        SurfaceImplConfiguration<Api> implConfiguration{
            .device = device.getBackendDevice(),
            .width = configuration.width,
            .height = configuration.height,
            .usage = configuration.usage,
            .format = configuration.format,
            .presentMode = configuration.presentMode,
            .maxFramesInFlight = configuration.maxFramesInFlight
        };

        auto optional = surface.configure(implConfiguration);
        if (optional.has_value()) {
            process.report(Api::toError(*optional));
        }
    }
private:
    explicit SurfaceImpl(Surface surface) : surface(std::move(surface)) {}
    Surface surface;

    friend class DeviceImpl<Api>;
    friend class InstanceImpl<Api>;
};

}