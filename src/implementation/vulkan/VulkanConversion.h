//
// Created by Nemesis Verstraete on 27/09/2026.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace renderium {
enum class RequestError;
enum class BufferUsages : uint16_t;
enum class TextureDimension;
struct Extent3D;
enum class CullMode;
enum class FrontFace;
enum class PrimitiveTopology;
enum class VertexFormat;
enum class VertexStepMode;
enum class TextureFormat;
enum class PresentMode;
enum class TextureUsages : uint8_t;
}

namespace rhi::vulkan {
enum class VulkanError;

class VulkanConversion {
public:
    static vk::SampleCountFlagBits mapSampleCount(uint32_t sampleCount);
    static vk::ImageType mapTextureDimension(renderium::TextureDimension dimension);
    static vk::Extent3D mapExtent3D(renderium::Extent3D extent3d);
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
    static vk::BufferUsageFlags mapBufferUsages(renderium::BufferUsages bufferUsages);

    // error
    static renderium::Error toError(VulkanError errorCode);
    static renderium::RequestError toRequestError(VulkanError errorCode);
};

}
