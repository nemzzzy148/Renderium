//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include "../src/utils/BitwiseOperations.h"

namespace renderium {

enum class TextureFormat {
    R8Unorm,

    RG8Unorm,

    RGBA8Unorm,
    RGBA8UnormSRGB,
    BGRA8Unorm,
    BGRA8UnormSRGB,

    RGBA16Float,
    RGBA32Float,

    Depth16Unorm,
    Depth24UnormStencil8Uint,
    Depth32Float,
    Depth32FloatStencil8Uint
};

enum class TextureUsages : uint8_t {
    None = 0,
    RenderTarget = 1,
    Sampled = 1 << 1,
    DepthStencil = 1 << 2,
    Storage = 1 << 3,
    TransferSource = 1 << 4,
    TransferDestination = 1 << 5
};

class Texture {
public:
protected:
};

class TextureView {

};

}

template<> struct utils::enumBitwiseOperations<renderium::TextureUsages> : std::true_type {};
