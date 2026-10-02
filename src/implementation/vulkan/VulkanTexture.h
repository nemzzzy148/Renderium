//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace renderium {
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
    static 
    VulkanTextureView createView(const renderium::TextureViewDescriptor& descriptor);
private:
    vk::raii::Image image = nullptr;
};

}
