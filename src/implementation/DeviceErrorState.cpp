//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "DeviceErrorState.h"

#include <string>

namespace rhi {

void DeviceErrorState::pushErrorScope(const renderium::ErrorFilter type) {
    std::lock_guard lockGuard(lock);
    // stack works in other direction, back is top
    errorStack.emplace_back(renderium::Error{type,"Something went wrong" });
}

void DeviceErrorState::report(const renderium::Error error) {
    std::lock_guard lockGuard(lock);
    for (auto&[type, message] : errorStack) {
        if (type == error.type) {
            message = std::move(error.message);
        }
    } // Goes through each element checking the same type, if found set the error message in the error stack.
}

renderium::Error DeviceErrorState::popErrorScope() {
    std::lock_guard lockGuard(lock);
    renderium::Error error = errorStack.back();
    errorStack.pop_back();
    return error;
}

}
