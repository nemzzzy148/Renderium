//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <functional>
#include <vulkan/vulkan_raii.hpp>

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
    explicit VulkanTexture(const renderium::TextureDescriptor& descriptor);
    VulkanTextureView createView(const renderium::TextureViewDescriptor& descriptor) const;

    ~VulkanTexture();
private:
    uint32_t memoryPoolIndex;
    uint32_t memoryIndex;
    std::function<void(uint32_t, uint32_t)> destructCallback;
    vk::raii::Image image = nullptr;
};

}
