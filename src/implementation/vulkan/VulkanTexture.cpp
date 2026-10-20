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

VulkanTextureView VulkanTexture::createView(const renderium::TextureViewDescriptor &descriptor) const {
    return VulkanTextureView::create(image, descriptor);
}

VulkanTexture::~VulkanTexture() {
    destructCallback(poolIdx, nodeIdx, memoryType); // calls the origin device's freeTexture() func
}

}