//
// Created by Nemesis Verstraete on 05/11/2026.
//

#include <Texture.h>

#include "utils/BitwiseOperations.h"

namespace renderium {

TextureView Texture::createView(const TextureViewDescriptor &descriptor) const {
    if (!resource) return TextureView(nullptr);

    TextureViewDescriptor desc = descriptor;
    if (descriptor.format == TextureFormat::None) {
        desc.format = format;
    }
    if (descriptor.usage == TextureUsages::None ) {
        desc.usage = usage;
    }
    return TextureView(resource->createView(desc));
}

Texture::Texture(std::shared_ptr<detail::TextureResource> resource, const TextureDescriptor &descriptor)
    : width(descriptor.size.width), height(descriptor.size.height),
    depthOrArrayLayers(descriptor.size.depthOrArrayLayers), mipLevelCount(descriptor.mipLevelCount),
    sampleCount(descriptor.sampleCount), dimension(descriptor.dimension), format(descriptor.format),
    usage(descriptor.usage), resource(std::move(resource)) {}

}