//
// Created by Nemesis Verstraete on 21/10/2026.
//

#pragma once
#include <Error.h>
#include <mutex>
#include <vector>

namespace renderium {
enum class ErrorFilter;
}

namespace rhi {

class DeviceErrorState {
public:
    void pushErrorScope(renderium::ErrorFilter type);
    void report(const renderium::Error& error);
    renderium::Error popErrorScope();
private:
    std::vector<renderium::Error> errorStack;
    std::mutex lock;
};

}
