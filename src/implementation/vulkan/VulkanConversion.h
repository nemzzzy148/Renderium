//
// Created by Nemesis Verstraete on 27/09/2026.
//

#pragma once
#include <vulkan/vulkan.hpp>

#include "../window/glfw/GlfwWindow.h"

namespace renderium {
enum class CullMode;
enum class FrontFace;
enum class PrimitiveTopology;
enum class VertexFormat;
enum class VertexStepMode;
}

namespace rhi::vulkan {

class VulkanConversion {
public:
    static vk::Format mapTextureFormat(renderium::TextureFormat textureFormat);
    static renderium::TextureFormat toTextureFormat(vk::Format format);
    static vk::PresentModeKHR mapPresentMode(renderium::PresentMode presentMode);
    static renderium::PresentMode toPresentMode(vk::PresentModeKHR presentMode);
    static vk::ImageUsageFlags mapTextureUsages(renderium::TextureUsages textureUsages);
    static renderium::TextureUsages toTextureUsages(vk::ImageUsageFlags usages);
    static vk::VertexInputRate mapVertexStepMode(renderium::VertexStepMode stepMode);
    static vk::Format mapVertexFormat(renderium::VertexFormat vertexFormat);
    static vk::PrimitiveTopology mapPrimitiveTopology(renderium::PrimitiveTopology primitiveTopology);
    static vk::FrontFace mapFrontFace(renderium::FrontFace frontFace);
    static vk::CullModeFlagBits mapCullMode(renderium::CullMode cullMode);
};

}
