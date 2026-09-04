//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once
#include <memory>

#include "API.h"
#include "Error.h"
#include "Result.h"
#include "Window.h"

namespace renderium {
struct WindowCreateInfo;
}

namespace rhi {

template<typename GraphicsApi, typename WindowApi>
using TemplateInternalSurfaceResult = renderium::Result<typename GraphicsApi::InternalSurface, typename WindowApi::Error>;

template<typename GraphicsApi, typename WindowApi>
TemplateInternalSurfaceResult<GraphicsApi, WindowApi> createSurface(const typename GraphicsApi::Instance& instance, void* windowHandle);

template<typename GraphicsApi>
using InternalSurfaceResult = renderium::Result<typename GraphicsApi::InternalSurface, Error>;

template<typename GraphicsApi>
InternalSurfaceResult<GraphicsApi> createSurface(const typename GraphicsApi::Instance& instance, const renderium::Window& window) {
    switch (window.getBackend()) {
        case renderium::WindowBackend::Glfw: {
            auto result = createSurface<GraphicsApi, window::GlfwApi>(instance, window.getNativeWindowHandle());
            if (!result.isOk()) {
                return InternalSurfaceResult<GraphicsApi>::err(Error::SurfaceCreationFailed);
            }
            return InternalSurfaceResult<GraphicsApi>::ok(result.unwrap());
        }
        default:
            return InternalSurfaceResult<GraphicsApi>::err(Error::SurfaceCreationFailed);
    }
}

namespace window::glfw {
enum class GlfwError;
}

template<typename GraphicsApi>
class InstanceImpl;

template<typename Api>
class WindowImpl : public renderium::Window::Impl {
public:
    using Window = Api::Window;
    using Error = Api::Error;
    using Result = renderium::Result<std::unique_ptr<WindowImpl>, Error>;
    static Result create(const renderium::WindowCreateInfo &createInfo) {
        auto result = Window::create(createInfo);
        if (!result.isOk()) {
            return Result::err(result.unwrapError());
        }
        return Result::ok(std::unique_ptr<WindowImpl>(new WindowImpl(std::move(result.unwrap()))));
    }

    void waitEvents() const override { window.waitEvents(); }
    void pollEvents() const override { window.pollEvents(); }

    [[nodiscard]] bool shouldClose() const override { return window.shouldClose(); }

    [[nodiscard]] void * getNativeWindowHandle() const override { return window.getNativeWindowHandle(); }
private:
    explicit WindowImpl(Window window) : window(std::move(window)) {}
    Window window;
};

}