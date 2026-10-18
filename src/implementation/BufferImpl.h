//
// Created by Nemesis Verstraete on 16/10/2026.
//

#pragma once

namespace rhi {

template<typename Api>
class BufferImpl {
public:
    using Buffer = Api::Buffer;
private:
    Buffer buffer;
};

}