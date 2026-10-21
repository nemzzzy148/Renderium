//
// Created by Nemesis Verstraete on 02/10/2026.
//

#include "VulkanTexture.h"

#include "Texture.h"
#include "VulkanConversion.h"

namespace rhi::vulkan {

// --- texture view ---

// --- texture ---

VulkanTexture::~VulkanTexture() {
    if (destructCallback) {
        destructCallback(poolIdx, nodeIdx, memoryType); // calls the origin device's freeTexture() func
    }
}

}