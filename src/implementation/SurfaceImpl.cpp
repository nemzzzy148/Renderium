//
// Created by Nemesis Verstraete on 31/10/2026.
//

#include "SurfaceImpl.h"

#include "API.h"
#include "vulkan/VulkanSurface.h"
#include "vulkan/VulkanTexture.h"

namespace rhi {

void validateSurfaceConfiguration(Process& process, const renderium::SurfaceConfiguration& configuration) {
    if (configuration.width == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The surface configuration requires a width greater than 0"
        });
    }
    if (configuration.height == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The surface configuration requires a height greater than 0."
        });
    }
    if (configuration.maxFramesInFlight == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The surface configuration requires the number of frames in flight to be greater than 0 (default is 3). "
            "Higher values increase stability at the cost of rendering latency, "
            "while lower values reduce latency but may cause frame drops depending on the hardware setup."
        });
    }
    if (configuration.usage == renderium::TextureUsages::None) {
        process.report({
            renderium::ErrorFilter::Validation,
            "The surface configuration must specifically have at least one TextureUsage. For standard rendering, the 'RenderingAttachment' should be included."
        });
    }
}

template<typename Api>
renderium::SurfaceCapabilities SurfaceImpl<Api>::getCapabilities(const renderium::Device &device) {
    const auto& deviceImpl = static_cast<const DeviceImpl<Api>&>(*device.impl);
    return surface.getCapabilities(deviceImpl.getBackendDevice());
}

template<typename Api>
bool SurfaceImpl<Api>::configure(const renderium::SurfaceConfiguration &configuration) {
    auto& device = static_cast<DeviceImpl<Api>&>(*configuration.device.impl);
    deviceState = device.deviceState;
    auto process = Process(device.getDeviceErrorState());
    validateSurfaceConfiguration(process, configuration);

    if (process.caughtError()) return true;

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
    return false;
}

template<typename Api>
std::shared_ptr<renderium::detail::TextureResource> SurfaceImpl<Api>::getCurrentTexture() {
    if (!deviceState) return nullptr; // no device state == not configured
    auto result = surface.getCurrentTexture();

    auto process = Process(deviceState->errorState);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        return nullptr;
    }

    auto allocation = deviceState->textures.allocate(std::unique_ptr<ITexture>(
        new TextureImpl<Api>(std::move(result.unwrap()), deviceState)));

    return std::shared_ptr<renderium::detail::TextureResource>(
        new TextureResource<Api>(deviceState, allocation));
}

template class SurfaceImpl<VulkanApi>;

}