//
// Created by Nemesis Verstraete on 02/10/2026.
//

#include "VulkanTexture.h"

#include "Texture.h"
#include "VulkanConversion.h"

namespace rhi::vulkan {

// --- texture view ---

VulkanTextureView VulkanTextureView::create(const vk::raii::Image &image,
    const renderium::TextureViewDescriptor &descriptor) {

}

// --- texture ---

VulkanTexture::TextureResult VulkanTexture::create(std::shared_ptr<VulkanDevice> device,
    const renderium::TextureDescriptor &descriptor) {
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
    auto image = vk::raii::Image(device->getDevice(), imageInfo);

    vk::MemoryRequirements memoryRequirements = image.getMemoryRequirements();


    // device memory for the image
}

VulkanTextureView VulkanTexture::createView(const renderium::TextureViewDescriptor &descriptor) const {
    return VulkanTextureView::create(image, descriptor);
}

VulkanTexture::~VulkanTexture() {
    destructCallback(allocation.pool, allocation.node);
}

}