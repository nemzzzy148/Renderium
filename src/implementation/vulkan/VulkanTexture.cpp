//
// Created by Nemesis Verstraete on 02/10/2026.
//

#include "VulkanTexture.h"

#include "Texture.h"
#include "VulkanConversion.h"

namespace rhi::vulkan {

VulkanTextureView VulkanTextureView::create(const vk::raii::Image &image,
    const renderium::TextureViewDescriptor &descriptor) {

}

VulkanTexture::VulkanTexture(const renderium::TextureDescriptor &descriptor) {
    const vk::ImageCreateInfo imageInfo {
        .imageType = VulkanConversion::mapTextureDimension(descriptor.dimension),
        .format = VulkanConversion::mapTextureFormat(descriptor.format),
        .extent = VulkanConversion::mapExtent3D(descriptor.size),
        .mipLevels = descriptor.mipLevelCount,
        .arrayLayers = 1,
        .samples = VulkanConversion::mapSampleCount(descriptor.sampleCount),
        .tiling = vk::ImageTiling::eOptimal,
        .usage = VulkanConversion::mapTextureUsages(descriptor.usages),
        .sharingMode = vk::SharingMode::eExclusive
    };
}

VulkanTextureView VulkanTexture::createView(const renderium::TextureViewDescriptor &descriptor) const {
    return VulkanTextureView::create(image, descriptor);
}

VulkanTexture::~VulkanTexture() {
    destructCallback(memoryPoolIndex, memoryIndex);
}

}