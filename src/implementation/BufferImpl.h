//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once
#include <algorithm>

#include "Buffer.h"

namespace rhi {

template<typename Api>
class BufferImpl : public renderium::Buffer::Impl {
public:
    using Buffer = Api::Buffer;
private:
    explicit BufferImpl(Buffer buffer, const uint64_t size, const renderium::BufferUsages usage,
        const renderium::BufferMapState mapState) : Impl(size, usage, mapState), buffer(std::move(buffer)) {}
    Buffer buffer;
};

}
