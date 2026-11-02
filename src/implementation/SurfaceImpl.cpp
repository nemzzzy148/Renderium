//
// Created by Nemesis Verstraete on 31/10/2026.
//

#include "SurfaceImpl.h"

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

}