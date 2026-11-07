//
// Created by Nemesis Verstraete on 02/10/2026.
//

#include "VulkanTexture.h"

#include "Texture.h"
#include "VulkanConversion.h"

namespace rhi::vulkan {

// --- texture view ---

// --- texture ---

VulkanTexture::TextureViewResult VulkanTexture::createView(const renderium::TextureViewDescriptor &descriptor) const {
    if (!device || image == nullptr) {
        return TextureViewResult::err(VulkanError::ImageViewCreationFailed);
    }

    const vk::ImageViewCreateInfo viewInfo {
        .image = image,
        .viewType = VulkanConversion::mapTextureViewDimension(descriptor.dimension),
        .format = VulkanConversion::mapTextureFormat(descriptor.format),
        .subresourceRange =
            {.aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = descriptor.baseMipLevel,
                .levelCount = descriptor.mipLevelCount,
                .baseArrayLayer = descriptor.baseArrayLayer,
                .layerCount = descriptor.arrayLayerCount
            }
    };

    auto [result, view] = device->getDevice().createImageView(viewInfo);
    if (result != vk::Result::eSuccess) {
        return TextureViewResult::err(VulkanError::ImageViewCreationFailed);
    }
    return TextureViewResult::ok(VulkanTextureView(
        std::make_unique<vk::raii::ImageView>(std::move(view))));
}

VulkanTexture::~VulkanTexture() {
    if (destructCallback) {
        destructCallback(poolIdx, nodeIdx, memoryType); // calls the origin device's freeTexture() func
    }
}

}