//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once
#include "Device.h"
#include "Surface.h"

namespace renderium {
enum class PresentMode;
enum class TextureFormat;
enum class Error;
}

namespace rhi {

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
    using Error = Api::Error;

    renderium::SurfaceCapabilities getCapabilities(const renderium::Device& device) override  {
        const auto& deviceImpl = static_cast<const DeviceImpl<Api>&>(*device.impl);
        return surface.getCapabilities(deviceImpl.getBackendDevice());
    }

    void configure(const renderium::SurfaceConfiguration& configuration) override {
        SurfaceImplConfiguration<Api> implConfiguration{
            .device = static_cast<const DeviceImpl<Api>&>(*configuration.device.impl).getBackendDevice(),
            .width = configuration.width,
            .height = configuration.height,
            .usage = configuration.usage,
            .format = configuration.format,
            .presentMode = configuration.presentMode,
            .maxFramesInFlight = configuration.maxFramesInFlight
        };
        surface.configure(implConfiguration);
    }
private:
    explicit SurfaceImpl(Surface surface) : surface(std::move(surface)) {}
    Surface surface;

    friend class DeviceImpl<Api>;
    friend class InstanceImpl<Api>;
};

}
