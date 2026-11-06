//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <memory>

#include "Resource.h"
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

enum class BufferMapState {
    Unmapped,
    Pending,
    Mapped
};

struct BufferDescriptor {
    uint64_t size;
    BufferUsages usage;
    bool mappedAtCreation = false;
};

class Buffer {
public:
    [[nodiscard]] uint64_t getSize() const { return size; }
    [[nodiscard]] BufferUsages getUsage() const { return usage; }
    // [[nodiscard]] BufferMapState mapState() const { return BufferMapState::Unmapped; }
private:
    explicit Buffer(std::shared_ptr<detail::Resource> resource, const BufferDescriptor& descriptor);

    uint64_t size;
    BufferUsages usage;

    std::shared_ptr<detail::Resource> resource;
    friend class Device;
};

}

template<> struct utils::enumBitwiseOperations<renderium::BufferUsages> : std::true_type {};