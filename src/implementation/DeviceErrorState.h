//
// Created by Nemesis Verstraete on 21/10/2026.
//

#pragma once
#include <Error.h>
#include <future>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>


namespace rhi {

class DeviceErrorState {
    static std::future<std::optional<renderium::Error>> createEmptyFuture();
public:
    void pushErrorScope(renderium::ErrorFilter filter);
    uint32_t createProcess();

    void report(uint32_t handle, const renderium::Error& error);
    void success(uint32_t handle);

    std::future<std::optional<renderium::Error>> popErrorScope();

private:
    struct Scope {
        explicit Scope(const renderium::ErrorFilter filter) : filter(filter) {}
        ~Scope() { promise.set_value(firstError); }

        renderium::ErrorFilter filter;
        std::promise<std::optional<renderium::Error>> promise;
        std::optional<renderium::Error> firstError;
    };
    std::vector<std::shared_ptr<Scope>> errorStack; // back is top
    std::unordered_map<uint32_t, std::vector<std::shared_ptr<Scope>>> processes;
    uint32_t counter = 1;
    std::mutex lock;
};

class Process {
public:
    explicit Process(DeviceErrorState& errorState) : errorState(&errorState) {
        handle = errorState.createProcess();
    }
    ~Process() {
        if (errorState) {
            errorState->success(handle);
        }
    }

    void report(const renderium::Error& error) {
        if (!errorState) return;
        errorState->report(handle, error);
        errorState = nullptr;
        reported = true;
    }
    void success() {
        if (!errorState) return;
        errorState->success(handle);
        errorState = nullptr;
    }
    [[nodiscard]] bool caughtError() const { return reported; }
private:
    bool reported = false;
    DeviceErrorState* errorState;
    uint32_t handle = 0;
};

}
