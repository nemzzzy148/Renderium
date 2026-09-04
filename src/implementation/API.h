//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include "Window.h"
#include "vulkan/vulkan.h"

namespace rhi {
namespace window {

namespace glfw {
enum class GlfwError;
class GlfwWindow;
}

struct GlfwApi {
    using Window = glfw::GlfwWindow;
    using Error = glfw::GlfwError;
};

template<renderium::WindowBackend backend>
struct windowBackendStruct;

template<> struct windowBackendStruct<renderium::WindowBackend::Glfw> { using type = GlfwApi; };

template<renderium::WindowBackend backend>
using WindowBackendType = windowBackendStruct<backend>::type;

}

namespace vulkan {
class VulkanQueue;
class VulkanSurface;
class VulkanDevice;
class VulkanDeviceSelector;
class VulkanInstance;
enum class VulkanError;
}

struct VulkanApi {
    using Instance = vulkan::VulkanInstance;
    using DeviceSelector = vulkan::VulkanDeviceSelector;
    using Device = vulkan::VulkanDevice;
    using Queue = vulkan::VulkanQueue;
    using InternalSurface = VkSurfaceKHR;
    using Surface = vulkan::VulkanSurface;
    using Error = vulkan::VulkanError;
};

}
