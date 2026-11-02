//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "DeviceErrorState.h"

#include <memory>
#include <ranges>

namespace rhi {

std::future<std::optional<renderium::Error>> DeviceErrorState::createEmptyFuture() {
    std::promise<std::optional<renderium::Error>> promise;
    promise.set_value(std::nullopt);
    return promise.get_future();
}

void DeviceErrorState::pushErrorScope(const renderium::ErrorFilter filter) {
    std::lock_guard lockGuard(lock);
    // stack works in other direction, back is top
    errorStack.emplace_back(std::make_shared<Scope>(filter));
}

uint32_t DeviceErrorState::createProcess() {
    std::lock_guard guard(lock);
    bool foundValidation = false; bool foundOutOfMemory = false; bool foundInternal = false;
    uint32_t process = counter++;
    std::vector<std::shared_ptr<Scope>> claimed;

    for (auto& scope : std::views::reverse(errorStack)) {
        bool* found = nullptr;

        switch (scope->filter) {
            case renderium::ErrorFilter::Validation: found = &foundValidation; break;
            case renderium::ErrorFilter::OutOfMemory: found = &foundOutOfMemory; break;
            case renderium::ErrorFilter::Internal: found = &foundInternal; break;
        }

        if (!found || *found) continue;
        *found = true;
        claimed.emplace_back(scope);
    }

    if (!claimed.empty()) processes.emplace(process, std::move(claimed));
    return process;
}

void DeviceErrorState::report(const uint32_t handle, const renderium::Error &error) {
    std::lock_guard guard(lock);
    const auto it = processes.find(handle);
    if (it == processes.end()) return;

    for (const auto& scope : it->second) {
        if (scope->filter == error.type && !scope->firstError) {
            scope->firstError = error;
        }
    }

    processes.erase(it);
}

void DeviceErrorState::success(const uint32_t handle) {
    std::lock_guard guard(lock);
    processes.erase(handle);
}

std::future<std::optional<renderium::Error>> DeviceErrorState::popErrorScope() {
    std::lock_guard guard(lock);
    if (errorStack.empty()) return createEmptyFuture();
    auto future = errorStack.back()->promise.get_future();
    errorStack.pop_back();
    return future;
}


}