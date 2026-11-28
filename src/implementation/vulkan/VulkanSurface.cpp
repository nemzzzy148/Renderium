//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanSurface.h"

#include <complex>

#include "Texture.h"
#include "VulkanConversion.h"
#include "VulkanInstance.h"
#include "VulkanQueue.h"
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

renderium::SurfaceCapabilities VulkanSurface::getCapabilities(const VulkanDevice& vulkanDevice) const {
    const auto& physicalDevice = vulkanDevice.getPhysicalDevice();

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

VulkanSurface::TextureResult VulkanSurface::getCurrentTexture() const {
    auto [imageResult, imageIndex] =
        swapChain.acquireNextImage(UINT64_MAX, nullptr, nullptr);

    if (imageResult == vk::Result::eErrorOutOfDateKHR) {
        return TextureResult::err(VulkanError::SwapChainOutOfDate);
    }
    if (imageResult != vk::Result::eSuccess) {
        return TextureResult::err(VulkanError::UnhandledException);
    }

    return TextureResult::ok(
        VulkanTexture(device, nullptr, MemoryAllocation(0, 0, 0, 0)));
}

std::shared_ptr<vk::raii::Semaphore> VulkanSurface::createSemaphore() const {
    auto [result, semaphore] = device->getDevice().createSemaphore({});
    if (result != vk::Result::eSuccess) {
        return nullptr;
    }
    return std::make_shared<vk::raii::Semaphore>(std::move(semaphore));
}

std::optional<VulkanError> VulkanSurface::destroySwapChain() {
    if (!device) return std::nullopt;
    if (const auto result = device->getDevice().waitIdle();
            result != vk::Result::eSuccess) {
        return VulkanError::FailedToWaitOnDevice;
            }
    return std::nullopt;
}

std::optional<VulkanError> VulkanSurface::createSwapChain(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    const vk::Format format = VulkanConversion::mapTextureFormat(configuration.format);
    const vk::ImageUsageFlags usage = VulkanConversion::mapTextureUsages(configuration.usage);
    const vk::PresentModeKHR presentMode = VulkanConversion::mapPresentMode(configuration.presentMode);

    auto& vulkanDevice = device->getDevice();

    auto [capabilitiesResult, capabilities]
        = device->getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    if (capabilitiesResult != vk::Result::eSuccess) {
        return VulkanError::FailedToGetSurfaceCapabilities;
    }

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

    auto [result, swpChn] = vulkanDevice.createSwapchainKHR(swapChainInfo);
    if (result != vk::Result::eSuccess) { // PANIC
        return VulkanError::SwapChainCreationFailed;
    }
    swapChain = std::move(swpChn);

    auto [imagesResult, img] = swapChain.getImages();
    if (imagesResult != vk::Result::eSuccess) { // PANIC
        return VulkanError::UnableToGetSwapChainImages;
    }
    images = std::move(img);

    presentCompleteSemaphores.clear();
    presentCompleteSemaphores.reserve(images.size());
    for (uint32_t i = 0; i < images.size(); i++) {
        auto semaphore = createSemaphore();
        if (!semaphore) return VulkanError::FailedToCreateSwapChainSemaphores;
        presentCompleteSemaphores.push_back(std::move(semaphore));
    }

    maxFramesInFlight = configuration.maxFramesInFlight;
    renderFinishedSemaphores.clear();
    renderFinishedSemaphores.reserve(maxFramesInFlight);
    for (uint32_t i = 0; i < maxFramesInFlight; i++) {
        auto semaphore = createSemaphore();
        if (!semaphore) return VulkanError::FailedToCreateSwapChainSemaphores;
        renderFinishedSemaphores.push_back(std::move(semaphore));
    }



    return std::nullopt;
}

std::optional<VulkanError> VulkanSurface::configure(const SurfaceImplConfiguration<VulkanApi> &configuration) {
    auto destroyResult = destroySwapChain();
    if (destroyResult.has_value()) return destroyResult;

    if (!device || &configuration.device != device.get()) {
        device = configuration.device.shared_from_this();
    } // Chooses new device when it does not have one / it is different.

    return createSwapChain(configuration);
}

VulkanSurface::ContextResult VulkanSurface::begin() {
    const auto fenceResult = device->getDevice().waitForFences(*fences[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
        return ContextResult::err(VulkanError::FailedToWaitForFence);
    }

    auto [imageResult, i] = swapChain.acquireNextImage(UINT64_MAX,
        *presentCompleteSemaphores[imageIndex], nullptr);

    if (imageResult == vk::Result::eErrorOutOfDateKHR) {
        return ContextResult::err(VulkanError::SwapChainOutOfDate);
    }
    if (imageResult != vk::Result::eSuccess) { // could not get next image index to render to
        return ContextResult::err(VulkanError::FailedToAcquireNextSwapChainImage);
    }

    imageIndex = i;
    if (device->getDevice().resetFences(*fences[frameIndex]) != vk::Result::eSuccess) {
        return ContextResult::err(VulkanError::FailedToResetFence);
    }

    return ContextResult::ok({.image = images[imageIndex],
        .presentCompleteSemaphore = presentCompleteSemaphores[imageIndex],
        .renderFinishedSemaphore = renderFinishedSemaphores[frameIndex]});
}

std::optional<VulkanError> VulkanSurface::end(const VulkanQueue &queue) {
    const vk::PresentInfoKHR info {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &**renderFinishedSemaphores[frameIndex],
        .swapchainCount = 1,
        .pSwapchains = &*swapChain,
        .pImageIndices = &imageIndex
    };

    const auto result = queue.getQueue().presentKHR(info);
    if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR) {
        return VulkanError::SwapChainOutOfDate;
    }
    if (result != vk::Result::eSuccess) {
        return VulkanError::SwapChainPresentationFailed;
    }

    frameIndex = (frameIndex + 1) % maxFramesInFlight;
    return std::nullopt;
}

}