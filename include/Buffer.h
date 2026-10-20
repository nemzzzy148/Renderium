//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <cstddef>

#include "../src/utils/BitwiseOperations.h"

namespace rhi {
template<typename Api>
class BufferImpl;
template<typename Api>
class DeviceImpl;
}

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

enum class BufferMapState {
    Unmapped,
    Pending,
    Mapped
};

struct BufferDescriptor {
    uint64_t size;
    BufferUsages usages;
    bool mappedAtCreation = false;
};

class Buffer {
public:
    [[nodiscard]] uint64_t size() const { return impl->size; }
    [[nodiscard]] BufferUsages usage() const { return impl->usage; }
    [[nodiscard]] BufferMapState mapState() const { return impl->mapState; }
private:
    struct Impl {
        ~Impl() = default;
        Impl(const uint64_t size, const BufferUsages usage, const BufferMapState mapState)
            : size(size), usage(usage), mapState(mapState) {}

        const uint64_t size;
        const BufferUsages usage;
        BufferMapState mapState;
    };

    std::unique_ptr<Impl> impl;
    explicit Buffer(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}

    friend class Device;

    template<typename Api>
    friend class rhi::BufferImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}

template<> struct utils::enumBitwiseOperations<renderium::BufferUsages> : std::true_type {};