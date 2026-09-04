//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanSurface.h"

#include "Texture.h"
#include "VulkanInstance.h"
#include "../WindowImpl.h"
#include "../Error.h"
#include "../SurfaceImpl.h"

namespace rhi::vulkan {
VulkanSurface::SurfaceResult VulkanSurface::create(const VulkanInstance& instance, const renderium::Window &window) {
    auto surfaceResult = createSurface<VulkanApi>(instance, window);
    if (!surfaceResult.isOk()) {
        return SurfaceResult::err(VulkanError::WindowBackendFailedToCreateSurface);
    }
    return SurfaceResult::ok(
        VulkanSurface(vk::raii::SurfaceKHR(instance.getHandle(), surfaceResult.unwrap())));
}

// possible window backends template

std::optional<renderium::TextureFormat> VulkanSurface::vulkanFormatConverter(const vk::Format format) {
    using F = renderium::TextureFormat;
    using VF = vk::Format;
    switch (format) {
        case VF::eR8Unorm: return F::R8Unorm;
        case VF::eR8G8Unorm: return F::RG8Unorm;
        case VF::eR8G8B8A8Unorm: return F::RGBA8Unorm;
        case VF::eR8G8B8A8Srgb: return F::RGBA8UnormSRGB;
        case VF::eB8G8R8A8Unorm: return F::BGRA8Unorm; // most common
        case VF::eB8G8R8A8Srgb: return F::BGRA8UnormSRGB;
        case VF::eR16G16B16A16Sfloat: return F::RGBA16Float;
        case VF::eR32G32B32A32Sfloat: return F::RGBA32Float;
        case VF::eD16Unorm: return F::Depth16Unorm;
        case VF::eD24UnormS8Uint: return F::Depth24UnormStencil8Uint;
        case VF::eD32Sfloat: return F::Depth32Float;
        case VF::eD32SfloatS8Uint: return F::Depth32FloatStencil8Uint;
        default: return std::nullopt;
    }
}

std::optional<renderium::PresentMode> VulkanSurface::vulkanPresentModeConverter(const vk::PresentModeKHR presentMode) {
    switch (presentMode) {
        case vk::PresentModeKHR::eImmediate: return renderium::PresentMode::Immediate;
        case vk::PresentModeKHR::eMailbox: return renderium::PresentMode::Mailbox;
        case vk::PresentModeKHR::eFifo: return renderium::PresentMode::VSync;
        case vk::PresentModeKHR::eFifoRelaxed: return renderium::PresentMode::Relaxed;
        default: return std::nullopt;
    }
}

renderium::TextureUsages VulkanSurface::vulkanUsageConverter(const vk::ImageUsageFlags usage) {
    using TU = renderium::TextureUsages;
    using IU = vk::ImageUsageFlagBits;
    auto finalUsages = renderium::TextureUsages::None;

    if (usage & IU::eColorAttachment)        finalUsages |= TU::RenderTarget;
    if (usage & IU::eDepthStencilAttachment) finalUsages |= TU::DepthStencil;
    if (usage & IU::eSampled)                finalUsages |= TU::Sampled;
    if (usage & IU::eStorage)                finalUsages |= TU::Storage;
    if (usage & IU::eTransferDst)            finalUsages |= TU::TransferDestination;
    if (usage & IU::eTransferSrc)            finalUsages |= TU::TransferSource;

    return finalUsages;
}

renderium::SurfaceCapabilities VulkanSurface::getCapabilities(const VulkanDevice& device) const {
    const auto& physicalDevice = device.getPhysicalDevice();

    // formats
    const std::vector<vk::SurfaceFormatKHR> vulkanFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    std::vector<renderium::TextureFormat> formats;
    for (const auto& vulkanFormat : vulkanFormats) {
        if (auto format = vulkanFormatConverter(vulkanFormat.format);
                format.has_value()) {
            formats.push_back(format.value());
                }
    }

    // present modes
    const std::vector<vk::PresentModeKHR> vulkanPresentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    std::vector<renderium::PresentMode> presentModes;
    for (const auto vulkanPresentMode : vulkanPresentModes) {
        if (auto presentMode = vulkanPresentModeConverter(vulkanPresentMode);
            presentMode.has_value()) {
            presentModes.push_back(presentMode.value());
            }
    }

    // usages
    const vk::SurfaceCapabilitiesKHR vulkanCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    const auto usages = vulkanUsageConverter(vulkanCapabilities.supportedUsageFlags);

    return {usages, std::move(formats), std::move(presentModes)};
}

void VulkanSurface::destroySwapChain(const VulkanDevice& device) {
    device.getDevice().waitIdle();
    images.clear();
    imageViews.clear();
}

vk::Format VulkanSurface::formatConverter(const renderium::TextureFormat format) {
    using F = renderium::TextureFormat;
    using VF = vk::Format;
    switch (format) {
        case F::R8Unorm: return VF::eR8Unorm;
        case F::RG8Unorm: return VF::eR8G8Unorm;
        case F::RGBA8Unorm: return VF::eR8G8B8A8Unorm;
        case F::RGBA8UnormSRGB: return VF::eR8G8B8A8Srgb;
        case F::BGRA8Unorm: return VF::eB8G8R8A8Unorm;
        case F::BGRA8UnormSRGB: return VF::eB8G8R8A8Srgb;
        case F::RGBA16Float: return VF::eR16G16B16A16Sfloat;
        case F::RGBA32Float: return VF::eR32G32B32A32Sfloat;
        case F::Depth16Unorm: return VF::eD16Unorm;
        case F::Depth24UnormStencil8Uint: return VF::eD24UnormS8Uint;
        case F::Depth32Float: return VF::eD32Sfloat;
        case F::Depth32FloatStencil8Uint: return VF::eD32SfloatS8Uint;
        default: return VF::eB8G8R8A8Unorm; // can't happen but makes compiler happy
    }
}

vk::PresentModeKHR VulkanSurface::presentModeConverter(const renderium::PresentMode presentMode) {
    using PM = renderium::PresentMode;
    using VPM = vk::PresentModeKHR;
    switch (presentMode) {
        case PM::Immediate: return VPM::eImmediate;
        case PM::VSync: return VPM::eFifo;
        case PM::Mailbox: return VPM::eMailbox;
        case PM::Relaxed: return VPM::eFifoRelaxed;
        default: return VPM::eFifo;
    }
}

vk::ImageUsageFlags VulkanSurface::usageConverter(const renderium::TextureUsages usages) {
    using IU = vk::ImageUsageFlagBits;
    using TU = renderium::TextureUsages;
    vk::ImageUsageFlags finalUsage = {};

    if (utils::any(usages & TU::RenderTarget)) finalUsage |= IU::eColorAttachment;
    if (utils::any(usages & TU::DepthStencil)) finalUsage |= IU::eDepthStencilAttachment;
    if (utils::any(usages & TU::Sampled)) finalUsage |= IU::eSampled;
    if (utils::any(usages & TU::Storage)) finalUsage |= IU::eStorage;
    if (utils::any(usages & TU::TransferDestination)) finalUsage |= IU::eTransferDst;
    if (utils::any(usages & TU::TransferSource)) finalUsage |= IU::eTransferSrc;

    return finalUsage;
}

void VulkanSurface::createSwapChain(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    const vk::Format format = formatConverter(configuration.format);
    const vk::ImageUsageFlags usage = usageConverter(configuration.usage);
    const vk::PresentModeKHR presentMode = presentModeConverter(configuration.presentMode);

    auto& device = configuration.device;
    const auto capabilities = device.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);

    const vk::SwapchainCreateInfoKHR swapChainInfo{
        .surface = surface,
        .minImageCount = configuration.maxFramesInFlight,
        .imageFormat = format,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = {.width = configuration.width, .height = configuration.height},
        .imageArrayLayers = 1,
        .imageUsage = usage,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = presentMode,
        .clipped = false,
        .oldSwapchain = nullptr
    };

    swapChain = vk::raii::SwapchainKHR(device.getDevice(), swapChainInfo);
    images = swapChain.getImages();
    vk::ImageViewCreateInfo imageViewInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };
    for (const auto& image : images) {
        imageViewInfo.image = image;
        imageViews.emplace_back(device.getDevice(), imageViewInfo);
    }

    if (!presentCompleteSemaphores.empty()) return;
    for (uint32_t i = 0; i < images.size(); i++) {
        presentCompleteSemaphores.emplace_back(device.getDevice(), vk::SemaphoreCreateInfo());
    }
}

void VulkanSurface::configure(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    destroySwapChain(configuration.device);
    createSwapChain(configuration);
}

}