//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <cstddef>

namespace renderium {

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual void destroy() = 0;
    [[nodiscard]] virtual size_t getSize() const = 0;
    virtual void write(const void* data, size_t size, size_t offset) = 0;
};

}
