//
// Created by Nemesis Verstraete on 17/08/2026.
//

#include "Window.h"

#include "implementation/API.h"
#include "implementation/WindowImpl.h"
#include "implementation/window/glfw/GlfwWindow.h"

namespace renderium {

Window::WindowResult Window::create(const WindowDescriptor& descriptor) {
    switch (descriptor.backend) {
        case WindowBackend::Glfw: {
            auto result = rhi::WindowImpl<rhi::window::GlfwApi>::create(descriptor);
            if (!result.isOk()) {
                return WindowResult::err(WindowError::WindowCreateError);
            }
            return WindowResult::ok(Window(result.unwrap(), descriptor.backend));
        }
    }
    return WindowResult::err(WindowError::WindowCreateError);
}

}
