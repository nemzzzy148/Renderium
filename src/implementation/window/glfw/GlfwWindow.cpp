//
// Created by Nemesis Verstraete on 17/08/2026.
//

#define GLFW_INCLUDE_VULKAN
#include "GlfwWindow.h"

#include "Window.h"
#include "../../API.h"
#include "../../Error.h"
#include "../../vulkan/VulkanInstance.h"
#include "../../WindowImpl.h"


namespace rhi {

using InternalVulkanSurfaceResult = TemplateInternalSurfaceResult<VulkanApi, window::GlfwApi>;

template<>
TemplateInternalSurfaceResult<VulkanApi, window::GlfwApi> createSurface<VulkanApi, window::GlfwApi>(
    const VulkanApi::Instance& instance,
    void* windowHandle) {
    const auto window = static_cast<GLFWwindow*>(windowHandle);
    VulkanApi::InternalSurface surface_ = VK_NULL_HANDLE;
    if (glfwCreateWindowSurface(*instance.getHandle(), window, nullptr, &surface_) != VK_SUCCESS) {
        return InternalVulkanSurfaceResult::err(window::glfw::GlfwError::FailedToCreateVulkanSurface);
    }
    return InternalVulkanSurfaceResult::ok(surface_);
}

namespace window::glfw {

GlfwWindow::WindowResult GlfwWindow::create(const renderium::WindowCreateInfo &createInfo) {
    if (createInfo.backend != renderium::WindowBackend::Glfw)
        return WindowResult::err(GlfwError::CreateInfoOtherBackend);
    if (!glfwInit()) return WindowResult::err(GlfwError::FailedToInit);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(static_cast<int>(createInfo.width), static_cast<int>(createInfo.height),
        createInfo.title.c_str(), nullptr, nullptr);
    if (!window) return WindowResult::err(GlfwError::FailedToCreateWindow);

    return WindowResult::ok(GlfwWindow(window));
}

}
}