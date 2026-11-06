//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanSurface.h"

#include <complex>

#include "Texture.h"
#include "VulkanConversion.h"
#include "VulkanInstance.h"
#include "VulkanTexture.h"
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
    auto [formatResult, vulkanFormats] = physicalDevice.getSurfaceFormatsKHR(surface);
    std::vector<renderium::TextureFormat> formats;
    if (formatResult == vk::Result::eSuccess) {
        formats.reserve(vulkanFormats.size());
        for (const auto&[format, colorSpace] : vulkanFormats) {
            formats.push_back(VulkanConversion::toTextureFormat(format));
        }
    }

    // present modes
    auto [presentResult, vulkanPresentModes] = physicalDevice.getSurfacePresentModesKHR(surface);
    std::vector<renderium::PresentMode> presentModes;
    if (presentResult == vk::Result::eSuccess) {
        presentModes.reserve(vulkanPresentModes.size());
        for (const auto vulkanPresentMode : vulkanPresentModes) {
            presentModes.push_back(VulkanConversion::toPresentMode(vulkanPresentMode));
        }
    }

    // usages
    auto [capabilitiesResult, vulkanCapabilities] = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    auto usages = renderium::TextureUsages::None;
    if (capabilitiesResult == vk::Result::eSuccess) {
        usages = VulkanConversion::toTextureUsages(vulkanCapabilities.supportedUsageFlags);
    }

    return {usages, std::move(formats), std::move(presentModes)};
}

VulkanSurface::TextureViewResult VulkanSurface::getCurrentTexture() {
    auto [imageResult, imageIndex] =
        swapChain.acquireNextImage(UINT64_MAX, nullptr, nullptr);

    if (imageResult == vk::Result::eErrorOutOfDateKHR) {
        return TextureViewResult::err(VulkanError::SwapChainOutOfDate);
    }
    if (imageResult != vk::Result::eSuccess) {
        return TextureViewResult::err(VulkanError::UnhandledException);
    }

    return TextureViewResult::ok(VulkanTextureView(views[imageIndex]));
}

void VulkanSurface::destroySwapChain(const VulkanDevice& device) {
    device.getDevice().waitIdle();
    images.clear();
}

std::optional<VulkanError> VulkanSurface::createSwapChain(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    const vk::Format format = VulkanConversion::mapTextureFormat(configuration.format);
    const vk::ImageUsageFlags usage = VulkanConversion::mapTextureUsages(configuration.usage);
    const vk::PresentModeKHR presentMode = VulkanConversion::mapPresentMode(configuration.presentMode);

    auto& device = configuration.device;

    auto [capabilitiesResult, capabilities]
        = device.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);

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

    auto [result, swpChn] = device.getDevice().createSwapchainKHR(swapChainInfo);
    if (result != vk::Result::eSuccess) { // PANIC
        return VulkanError::SwapChainCreationFailed;
    }
    swapChain = std::move(swpChn);

    auto [imagesResult, img] = swapChain.getImages();
    if (imagesResult != vk::Result::eSuccess) { // PANIC
        return VulkanError::UnableToGetSwapChainImages;
    }
    images = std::move(img);

    vk::ImageViewCreateInfo viewInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };
    views.reserve(images.size());
    for (const auto &image : images) {
        viewInfo.image = image;
        auto [viewResult, view] = device.getDevice().createImageView(viewInfo);
        if (viewResult != vk::Result::eSuccess) {
            return VulkanError::FailedToCreateSwapChainImageViews;
        }
        views.emplace_back(std::make_shared<vk::raii::ImageView>(std::move(view)));
    }


    if (!presentCompleteSemaphores.empty()) return std::nullopt;
    for (uint32_t i = 0; i < images.size(); i++) {
        auto [semaphoreResult, semaphore] = device.getDevice().createSemaphore(vk::SemaphoreCreateInfo());
        if (semaphoreResult != vk::Result::eSuccess) { // PANIC
            return VulkanError::FailedToCreateSwapChainSemaphores;
        }
        presentCompleteSemaphores.emplace_back(std::move(semaphore));
    }

    return std::nullopt;
}

std::optional<VulkanError> VulkanSurface::configure(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    destroySwapChain(configuration.device);
    return createSwapChain(configuration);
}

}