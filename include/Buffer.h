//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <cstddef>
#include "../src/utils/BitwiseOperations.h"

namespace renderium {

enum class BufferUsages : uint16_t {
    None = 0,
    MapRead = 1 << 1,
    MapWrite = 1 << 2,
    CopySrc = 1 << 3,
    CopyDst = 1 << 4,
    Index = 1 << 5,
    Vertex = 1 << 6,
    Uniform = 1 << 7,
    Storage = 1 << 8,
    Indirect = 1 << 9,
    QueryResolve = 1 << 10
};

struct BufferDescriptor {
    uint64_t size;
    BufferUsages usages;
    bool mappedAtCreation = false;
};

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual void destroy() = 0;
    [[nodiscard]] virtual size_t getSize() const = 0;
    virtual void write(const void* data, size_t size, size_t offset) = 0;
};

}

template<> struct utils::enumBitwiseOperations<renderium::BufferUsages> : std::true_type {};