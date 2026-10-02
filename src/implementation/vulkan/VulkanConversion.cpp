//
// Created by Nemesis Verstraete on 27/09/2026.
//

#include "VulkanConversion.h"

#include "Primitive.h"
#include "Texture.h"
#include "Vertex.h"

namespace rhi::vulkan {
vk::Format VulkanConversion::mapTextureFormat(const renderium::TextureFormat textureFormat) {
    switch (textureFormat) {
        case renderium::TextureFormat::R8Unorm: return vk::Format::eR8Unorm;
        case renderium::TextureFormat::RG8Unorm: return vk::Format::eR8G8Unorm;
        case renderium::TextureFormat::RGBA8Unorm: return vk::Format::eR8G8B8A8Unorm;
        case renderium::TextureFormat::RGBA8UnormSRGB: return vk::Format::eR8G8B8A8Srgb;
        case renderium::TextureFormat::BGRA8Unorm: return vk::Format::eB8G8R8A8Unorm;
        case renderium::TextureFormat::BGRA8UnormSRGB: return vk::Format::eB8G8R8A8Srgb;
        case renderium::TextureFormat::RGBA16Float: return vk::Format::eR16G16B16A16Sfloat;
        case renderium::TextureFormat::RGBA32Float: return vk::Format::eR32G32B32A32Sfloat;
        case renderium::TextureFormat::Depth16Unorm: return vk::Format::eD16Unorm;
        case renderium::TextureFormat::Depth24UnormStencil8Uint: return vk::Format::eD24UnormS8Uint;
        case renderium::TextureFormat::Depth32Float: return vk::Format::eD32Sfloat;
        case renderium::TextureFormat::Depth32FloatStencil8Uint: return vk::Format::eD32SfloatS8Uint;
        default: return vk::Format::eB8G8R8A8Unorm; // Can't happen but makes compiler happy
    }
}

renderium::TextureFormat VulkanConversion::toTextureFormat(const vk::Format format) {
    switch (format) {
        case vk::Format::eR8Unorm: return renderium::TextureFormat::R8Unorm;
        case vk::Format::eR8G8Unorm: return renderium::TextureFormat::RG8Unorm;
        case vk::Format::eR8G8B8A8Unorm: return renderium::TextureFormat::RGBA8Unorm;
        case vk::Format::eR8G8B8A8Srgb: return renderium::TextureFormat::RGBA8UnormSRGB;
        case vk::Format::eB8G8R8A8Unorm: return renderium::TextureFormat::BGRA8Unorm; // most common
        case vk::Format::eB8G8R8A8Srgb: return renderium::TextureFormat::BGRA8UnormSRGB;
        case vk::Format::eR16G16B16A16Sfloat: return renderium::TextureFormat::RGBA16Float;
        case vk::Format::eR32G32B32A32Sfloat: return renderium::TextureFormat::RGBA32Float;
        case vk::Format::eD16Unorm: return renderium::TextureFormat::Depth16Unorm;
        case vk::Format::eD24UnormS8Uint: return renderium::TextureFormat::Depth24UnormStencil8Uint;
        case vk::Format::eD32Sfloat: return renderium::TextureFormat::Depth32Float;
        case vk::Format::eD32SfloatS8Uint: return renderium::TextureFormat::Depth32FloatStencil8Uint;
        default: return renderium::TextureFormat::BGRA8Unorm; // will not happen
    }
}

vk::PresentModeKHR VulkanConversion::mapPresentMode(const renderium::PresentMode presentMode) {
    switch (presentMode) {
        case renderium::PresentMode::Immediate: return vk::PresentModeKHR::eImmediate;
        case renderium::PresentMode::VSync: return vk::PresentModeKHR::eFifo;
        case renderium::PresentMode::Mailbox: return vk::PresentModeKHR::eMailbox;
        case renderium::PresentMode::Relaxed: return vk::PresentModeKHR::eFifoRelaxed;
        default: return vk::PresentModeKHR::eFifo; // will never happen but makes compiler happy
    }
}

renderium::PresentMode VulkanConversion::toPresentMode(const vk::PresentModeKHR presentMode) {
    switch (presentMode) {
        case vk::PresentModeKHR::eImmediate: return renderium::PresentMode::Immediate;
        case vk::PresentModeKHR::eMailbox: return renderium::PresentMode::Mailbox;
        case vk::PresentModeKHR::eFifo: return renderium::PresentMode::VSync;
        case vk::PresentModeKHR::eFifoRelaxed: return renderium::PresentMode::Relaxed;
        default: return renderium::PresentMode::VSync; // will not happen
    }
}

vk::ImageUsageFlags VulkanConversion::mapTextureUsages(const renderium::TextureUsages textureUsages) {
    vk::ImageUsageFlags finalUsage = {};
    if (utils::any(textureUsages & renderium::TextureUsages::RenderAttachment)) finalUsage |= vk::ImageUsageFlagBits::eColorAttachment;
    if (utils::any(textureUsages & renderium::TextureUsages::TransientAttachment)) finalUsage |= vk::ImageUsageFlagBits::eTransientAttachment;
    if (utils::any(textureUsages & renderium::TextureUsages::TextureBinding)) finalUsage |= vk::ImageUsageFlagBits::eSampled;
    if (utils::any(textureUsages & renderium::TextureUsages::StorageBinding)) finalUsage |= vk::ImageUsageFlagBits::eStorage;
    if (utils::any(textureUsages & renderium::TextureUsages::CopyDst)) finalUsage |= vk::ImageUsageFlagBits::eTransferDst;
    if (utils::any(textureUsages & renderium::TextureUsages::CopySrc)) finalUsage |= vk::ImageUsageFlagBits::eTransferSrc;
    return finalUsage;
}

renderium::TextureUsages VulkanConversion::toTextureUsages(const vk::ImageUsageFlags usages) {
    auto finalUsages = renderium::TextureUsages::None;
    if (usages & vk::ImageUsageFlagBits::eColorAttachment) finalUsages |= renderium::TextureUsages::RenderAttachment;
    if (usages & vk::ImageUsageFlagBits::eSampled) finalUsages |= renderium::TextureUsages::TextureBinding;
    if (usages & vk::ImageUsageFlagBits::eStorage) finalUsages |= renderium::TextureUsages::StorageBinding;
    if (usages & vk::ImageUsageFlagBits::eTransferDst) finalUsages |= renderium::TextureUsages::CopyDst;
    if (usages & vk::ImageUsageFlagBits::eTransferSrc) finalUsages |= renderium::TextureUsages::CopySrc;
    if (usages & vk::ImageUsageFlagBits::eTransientAttachment) finalUsages |= renderium::TextureUsages::TransientAttachment;
    return finalUsages;
}

vk::VertexInputRate VulkanConversion::mapVertexStepMode(const renderium::VertexStepMode stepMode) {
    switch (stepMode) {
        case renderium::VertexStepMode::Vertex: return vk::VertexInputRate::eVertex;
        case renderium::VertexStepMode::Instance: return vk::VertexInputRate::eInstance;
        default: return vk::VertexInputRate::eVertex; // will never happen but makes compiler happy
    }
}

vk::Format VulkanConversion::mapVertexFormat(const renderium::VertexFormat vertexFormat) {
    switch (vertexFormat) {
        case renderium::VertexFormat::Float32: return vk::Format::eR32Sfloat;
        case renderium::VertexFormat::Float32x2: return vk::Format::eR32G32Sfloat;
        case renderium::VertexFormat::Float32x3: return vk::Format::eR32G32B32Sfloat;
        case renderium::VertexFormat::Float32x4: return vk::Format::eR32G32B32A32Sfloat;
        default: return vk::Format::eR32G32B32Sfloat; // will never happen but makes compiler happy
    }
}

vk::PrimitiveTopology VulkanConversion::mapPrimitiveTopology(const renderium::PrimitiveTopology primitiveTopology) {
    switch (primitiveTopology) {
        case renderium::PrimitiveTopology::PointList: return vk::PrimitiveTopology::ePointList;
        case renderium::PrimitiveTopology::LineList: return vk::PrimitiveTopology::eLineList;
        case renderium::PrimitiveTopology::LineStrip: return vk::PrimitiveTopology::eLineStrip;
        case renderium::PrimitiveTopology::TriangleList: return vk::PrimitiveTopology::eTriangleList;
        case renderium::PrimitiveTopology::TriangleStrip: return vk::PrimitiveTopology::eTriangleStrip;
        default: return vk::PrimitiveTopology::eTriangleList; // will never happen but makes compiler happy
    }
}

vk::FrontFace VulkanConversion::mapFrontFace(const renderium::FrontFace frontFace) {
    switch (frontFace) {
        case renderium::FrontFace::CW: return vk::FrontFace::eClockwise;
        default: return vk::FrontFace::eCounterClockwise;
    }
}

vk::CullModeFlagBits VulkanConversion::mapCullMode(const renderium::CullMode cullMode) {
    switch (cullMode) {
        case renderium::CullMode::Back: return vk::CullModeFlagBits::eBack;
        case renderium::CullMode::Front: return vk::CullModeFlagBits::eFront;
        case renderium::CullMode::None: return vk::CullModeFlagBits::eNone;
        default: return vk::CullModeFlagBits::eBack; // will never happen but makes compiler happy
    }
}

}