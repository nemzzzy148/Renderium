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
class VulkanQueue;
class VulkanTexture;
class VulkanDevice;
enum class VulkanError;
class VulkanInstance;

struct SwapchainContext {
    vk::Image image = nullptr;
    std::shared_ptr<vk::raii::Semaphore> presentCompleteSemaphore = nullptr;
    std::shared_ptr<vk::raii::Semaphore> renderFinishedSemaphore = nullptr;
};

class VulkanSurface {
public:
    using SurfaceResult = renderium::Result<VulkanSurface, VulkanError>;
    static SurfaceResult create(const VulkanInstance& instance, const renderium::Window& window);

    [[nodiscard]] renderium::SurfaceCapabilities getCapabilities(const VulkanDevice& vulkanDevice) const;

    using TextureResult = renderium::Result<VulkanTexture, VulkanError>;
    [[nodiscard]] TextureResult getCurrentTexture() const;
private:
    [[nodiscard]] std::shared_ptr<vk::raii::Semaphore> createSemaphore() const; // return nullptr == fail
    std::optional<VulkanError> destroySwapChain();
    std::optional<VulkanError> createSwapChain(const SurfaceImplConfiguration<VulkanApi>& configuration);
public:
    std::optional<VulkanError> configure(const SurfaceImplConfiguration<VulkanApi>& configuration);

    using ContextResult = renderium::Result<SwapchainContext, VulkanError>;
    ContextResult begin();
    std::optional<VulkanError> end(const VulkanQueue& queue);
private:
    explicit VulkanSurface(vk::raii::SurfaceKHR surface) : surface(std::move(surface)) {}

    vk::raii::SurfaceKHR surface;
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> images;

    uint32_t maxFramesInFlight = 3;
    uint32_t frameIndex = 0;
    uint32_t imageIndex = 0;
    std::vector<vk::raii::Fence> fences;
    std::vector<std::shared_ptr<vk::raii::Semaphore>> presentCompleteSemaphores;
    std::vector<std::shared_ptr<vk::raii::Semaphore>> renderFinishedSemaphores;

    std::shared_ptr<const VulkanDevice> device;

    friend class VulkanDevice;
};

}