//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "TextureImpl.h"

#include <bit>
#include <cmath>
#include <algorithm>

namespace rhi {

bool validateTextureDescriptor(Process& process, const renderium::TextureDescriptor& descriptor) {
    if (descriptor.size.width == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Texture creation requires the width of the texture to be greater than 0."
        });
    }
    if (descriptor.size.height == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Texture creation requires the width of the texture to be greater than 0."
        });
    }
    if (descriptor.size.depthOrArrayLayers == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Texture creation requires the number of depth or array layers to be greater than 0. "
            "This value defines the number of texture sliced or lor layers stored consecutively within the asset."
        });
    }
    if (const uint32_t maxMipLevelCount = static_cast<uint32_t>(std::floor(
        log2(std::max({descriptor.size.width, descriptor.size.height, descriptor.size.depthOrArrayLayers})))) + 1;
        descriptor.mipLevelCount > maxMipLevelCount) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Texture creation caps the number of mip levels to a maximum value based on its dimensions. "
            "This limit is calculated as the base-2 logarithm of the largest dimension plus one."
        });
    }
    if (descriptor.sampleCount == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Texture creation requires the sample count to be greater than 0. "
            "Standard textures default to a sample count of 1."
        });
    }
    if (descriptor.sampleCount > 1) {
        if (!std::has_single_bit(descriptor.sampleCount)) {
            process.report({
                renderium::ErrorFilter::Validation,
                "Texture creation requires a valid multisample count (typically 1, 2, 4, or 8). "
                "A count of 1 disables multisampling, while higher power-of-two values enable multisampling anti-aliasing (MSAA)."
            });
        }
        if (descriptor.mipLevelCount != 1) {
            process.report({
                renderium::ErrorFilter::Validation,
                "Texture creation requires the number of mip levels to be 1 when multisampling is enabled."
            });
        }
        if (descriptor.size.depthOrArrayLayers != 1) {
            process.report({
                renderium::ErrorFilter::Validation,
                "Texture creation requires the number of depth or array layers to be 1 when multisampling is enabled."
            });
        }
        if (descriptor.dimension != renderium::TextureDimension::D2) {
            process.report({
                renderium::ErrorFilter::Validation,
                "Texture creation requires the dimensions of the texture to equal 1 be multisampling is enabled."
            });
        }
    }
    
}

}