//
// Created by Nemesis Verstraete on 05/11/2026.
//

#include <Texture.h>

#include "utils/BitwiseOperations.h"

namespace renderium {

Texture::TextureViewResult Texture::createView(const TextureDescriptor &descriptor) {
}

Texture::Texture(std::shared_ptr<detail::Resource> resource, const TextureDescriptor &descriptor)
    : width(descriptor.size.width), height(descriptor.size.height),
    depthOrArrayLayers(descriptor.size.depthOrArrayLayers), mipLevelCount(descriptor.mipLevelCount),
    sampleCount(descriptor.sampleCount), dimension(descriptor.dimension), format(descriptor.format),
    usage(descriptor.usage), resource(std::move(resource)) {}

}