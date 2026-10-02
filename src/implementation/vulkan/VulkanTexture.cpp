//
// Created by Nemesis Verstraete on 02/10/2026.
//

#include "VulkanTexture.h"

#include "Texture.h"
#include "VulkanConversion.h"

namespace rhi::vulkan {

VulkanTextureView VulkanTextureView::create(const vk::raii::Image &image,
    const renderium::TextureViewDescriptor &descriptor) {
    const vk::ImageCreateInfo imageInfo {
        .arrayLayers = descriptor.arrayLayerCount,
        .format = VulkanConversion::mapTextureFormat(descriptor.format),
        .imageType =
    };
}

}
