//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once
#include <memory>
#include <string>

#include "Result.h"

namespace rhi {
template<typename Api>
class WindowImpl;
template<typename Api>
class SurfaceImpl;
template<typename Api>
class InstanceImpl;
}

namespace renderium {

enum class WindowError {
    WindowCreateError
};

enum class WindowBackend {
    Glfw
};

struct WindowCreateInfo {
    WindowBackend backend = WindowBackend::Glfw;
    std::string title = "Renderium Window";
    uint32_t width = 800;
    uint32_t height = 600;
};

class Window {
public:
    using WindowResult = Result<Window, WindowError>;
    static WindowResult create(const WindowCreateInfo &createInfo);

    void waitEvents() const { impl->waitEvents(); }
    void pollEvents() const { impl->pollEvents(); }

    [[nodiscard]] bool shouldClose() const { return impl->shouldClose(); }

    [[nodiscard]] WindowBackend getBackend() const { return backend; }
    [[nodiscard]] void* getNativeWindowHandle() const { return impl->getNativeWindowHandle(); }
private:
    const WindowBackend backend;

    struct Impl {
        virtual ~Impl() = default;

        virtual void waitEvents() const = 0;
        virtual void pollEvents() const = 0;

        [[nodiscard]] virtual bool shouldClose() const = 0;

        [[nodiscard]] virtual void* getNativeWindowHandle() const = 0;
    };
    explicit Window(std::unique_ptr<Impl> impl, const WindowBackend backend) : backend(backend), impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::WindowImpl;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    template<typename Api>
    friend class rhi::InstanceImpl;
};

}
