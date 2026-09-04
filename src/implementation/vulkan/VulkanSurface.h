//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "Result.h"

namespace renderium {
enum class PresentMode;
enum class TextureFormat;
enum class TextureUsages : uint8_t;
enum class WindowBackend;
struct SurfaceConfiguration;
class Window;
struct SurfaceCapabilities;
}

namespace rhi {
template<typename Api> struct SurfaceImplConfiguration;
struct VulkanApi;
}

namespace rhi::vulkan {
class VulkanDevice;
enum class VulkanError;
class VulkanInstance;

class VulkanSurface {
public:
    using SurfaceResult = renderium::Result<VulkanSurface, VulkanError>;
    static SurfaceResult create(const VulkanInstance& instance, const renderium::Window& window);
private:
    static std::optional<renderium::TextureFormat> vulkanFormatConverter(vk::Format format);
    static std::optional<renderium::PresentMode> vulkanPresentModeConverter(vk::PresentModeKHR presentMode);
    static renderium::TextureUsages vulkanUsageConverter(vk::ImageUsageFlags usage);
public:
    [[nodiscard]] renderium::SurfaceCapabilities getCapabilities(const VulkanDevice& device) const;
private:
    void destroySwapChain(const VulkanDevice& device);
    static vk::Format formatConverter(renderium::TextureFormat format);
    static vk::PresentModeKHR presentModeConverter(renderium::PresentMode presentMode);
    static vk::ImageUsageFlags usageConverter(renderium::TextureUsages usages);
    void createSwapChain(const SurfaceImplConfiguration<VulkanApi>& configuration);
public:
    void configure(const SurfaceImplConfiguration<VulkanApi>& configuration);
private:
    explicit VulkanSurface(vk::raii::SurfaceKHR surface) : surface(std::move(surface)) {}
    vk::raii::SurfaceKHR surface;

    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;

    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;

    friend class VulkanDevice;
};

}