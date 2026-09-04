//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once
#include "Result.h"
#include "../../InstanceImpl.h"
#include "GLFW/glfw3.h"

namespace renderium {
struct WindowCreateInfo;
}

namespace rhi::window::glfw {
enum class GlfwError;

class GlfwWindow {
public:
    using WindowResult = renderium::Result<GlfwWindow, GlfwError>;
    static WindowResult create(const renderium::WindowCreateInfo &createInfo);

    void waitEvents() const { glfwWaitEvents(); }
    void pollEvents() const { glfwPollEvents(); }

    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(window); }

    [[nodiscard]] void* getNativeWindowHandle() const { return window; }
private:
    explicit GlfwWindow(GLFWwindow* window) : window(window) {}

    GLFWwindow* window;
};

}
