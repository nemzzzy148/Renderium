//
// Created by Nemesis Verstraete on 17/08/2026.
//

#include "Surface.h"

namespace renderium {

SurfaceCapabilities Surface::getCapabilities(const Device &device) const {
    return impl->getCapabilities(device);
}

void Surface::configure(const SurfaceConfiguration &configuration) {
    if (!impl->configure(configuration)) {
        currentTextureState = {
            .size = {configuration.width, configuration.height, 1},
            .mipLevelCount = 1,
            .sampleCount = 1,
            .dimension = TextureDimension::D2,
            .format = configuration.format,
            .usage = configuration.usage,
            .viewFormats = {configuration.format}
        };
    }
}

SurfaceTexture Surface::getCurrentTexture() const {
    return {Texture(impl->getCurrentTexture(), currentTextureState)};

}

}
