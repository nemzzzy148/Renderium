//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "Result.h"
#include "VulkanSurface.h"
#include "Window.h"

namespace renderium {
struct DeviceDescriptor;
struct InstanceDescriptor;
}

namespace rhi::vulkan {
enum class VulkanError;

class VulkanInstance {
public:
    using InstanceResult = renderium::Result<VulkanInstance, VulkanError>;
    static InstanceResult create(const renderium::InstanceDescriptor& descriptor);

    [[nodiscard]] const vk::raii::Instance& getHandle() const { return instance; }
private:
    VulkanInstance(vk::raii::Context context, vk::raii::Instance instance)
        : context(std::move(context)), instance(std::move(instance)) {}

    static std::vector<const char*> getRequiredSurfaceExtensions();
    static std::vector<char const*> validationLayers;

    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;

    friend class VulkanAdapter;
    friend class VulkanSurface;
};

}