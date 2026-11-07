//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "Resource.h"
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
class VulkanTextureView;
class VulkanDevice;
enum class VulkanError;
class VulkanInstance;

class VulkanSurface {
public:
    using SurfaceResult = renderium::Result<VulkanSurface, VulkanError>;
    static SurfaceResult create(const VulkanInstance& instance, const renderium::Window& window);

    [[nodiscard]] renderium::SurfaceCapabilities getCapabilities(const VulkanDevice& vulkanDevice) const;

    using TextureViewResult = renderium::Result<VulkanTextureView, VulkanError>;
    TextureViewResult getCurrentTexture();
private:
    void destroySwapChain();
    std::optional<VulkanError> createSwapChain(const SurfaceImplConfiguration<VulkanApi>& configuration);
public:
    std::optional<VulkanError> configure(const SurfaceImplConfiguration<VulkanApi>& configuration);
private:
    explicit VulkanSurface(vk::raii::SurfaceKHR surface) : surface(std::move(surface)) {}

    vk::raii::SurfaceKHR surface;
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;

    vk::raii::Fence fence = nullptr;
    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;

    std::shared_ptr<const VulkanDevice> device;

    friend class VulkanDevice;
};

}