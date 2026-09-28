//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanSurface.h"

#include "Texture.h"
#include "VulkanConversion.h"
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

renderium::SurfaceCapabilities VulkanSurface::getCapabilities(const VulkanDevice& device) const {
    const auto& physicalDevice = device.getPhysicalDevice();

    // formats
    const std::vector<vk::SurfaceFormatKHR> vulkanFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    std::vector<renderium::TextureFormat> formats;
    formats.reserve(vulkanFormats.size());
    for (const auto&[format, colorSpace] : vulkanFormats) {
        formats.push_back(VulkanConversion::toTextureFormat(format));
    }

    // present modes
    const std::vector<vk::PresentModeKHR> vulkanPresentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    std::vector<renderium::PresentMode> presentModes;
    presentModes.reserve(vulkanPresentModes.size());
    for (const auto vulkanPresentMode : vulkanPresentModes) {
        presentModes.push_back(VulkanConversion::toPresentMode(vulkanPresentMode));
    }

    // usages
    const vk::SurfaceCapabilitiesKHR vulkanCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    const auto usages = VulkanConversion::toTextureUsages(vulkanCapabilities.supportedUsageFlags);

    return {usages, std::move(formats), std::move(presentModes)};
}

void VulkanSurface::destroySwapChain(const VulkanDevice& device) {
    device.getDevice().waitIdle();
    images.clear();
    imageViews.clear();
}

void VulkanSurface::createSwapChain(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    const vk::Format format = VulkanConversion::mapTextureFormat(configuration.format);
    const vk::ImageUsageFlags usage = VulkanConversion::mapTextureUsages(configuration.usage);
    const vk::PresentModeKHR presentMode = VulkanConversion::mapPresentMode(configuration.presentMode);

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