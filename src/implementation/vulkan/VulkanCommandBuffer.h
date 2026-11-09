//
// Created by Nemesis Verstraete on 09/11/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"

namespace rhi::vulkan {
enum class VulkanError;

class VulkanCommandBuffer {
public:
    using CommandBufferResult = renderium::Result<VulkanCommandBuffer, VulkanError>;
    static CommandBufferResult create();
private:

};

}
