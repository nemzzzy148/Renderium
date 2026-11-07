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

template<typename Api>
class SurfaceTextureResource final : public renderium::detail::Resource {

};

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

    renderium::SurfaceCapabilities getCapabilities(const renderium::Device& device) override;
    bool configure(const renderium::SurfaceConfiguration& configuration) override;
    std::shared_ptr<renderium::detail::Resource> getCurrentTexture() override;
private:
    explicit SurfaceImpl(Surface surface, std::shared_ptr<DeviceState<Api>> deviceState)
        : surface(std::move(surface)) {}

    Surface surface;

    friend class DeviceImpl<Api>;
    friend class InstanceImpl<Api>;
};

}