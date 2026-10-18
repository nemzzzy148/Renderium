//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <functional>
#include <utility>
#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace renderium {
struct TextureDescriptor;
struct TextureViewDescriptor;
}

namespace rhi::vulkan {


class VulkanTextureView {
public:
    static VulkanTextureView create(const vk::raii::Image& image, const renderium::TextureViewDescriptor& descriptor);
private:
    explicit VulkanTextureView(vk::raii::ImageView view) : view(std::move(view)) {}
    vk::raii::ImageView view = nullptr;
};

class VulkanTexture {
public:
    using TextureResult = renderium::Result<VulkanTexture, VulkanError>;

    static TextureResult create(std::shared_ptr<VulkanDevice> device, const renderium::TextureDescriptor& descriptor);
    VulkanTextureView createView(const renderium::TextureViewDescriptor& descriptor) const;

    ~VulkanTexture();

    static constexpr vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
private:
    explicit VulkanTexture(std::shared_ptr<VulkanDevice> device,
        std::function<void(uint32_t, uint32_t)> destructCallback, const MemoryAllocation allocation)
        : destructCallback(std::move(destructCallback)), device(std::move(device)), allocation(allocation) {}

    std::function<void(uint32_t, uint32_t)> destructCallback;
    vk::raii::Image image = nullptr;
    std::shared_ptr<VulkanDevice> device;

    MemoryAllocation allocation;
};

}
