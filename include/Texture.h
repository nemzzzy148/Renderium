//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include "../src/utils/BitwiseOperations.h"

namespace renderium {

struct Extent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLaters;
};

enum class TextureDimension  {
    D1,
    D2,
    D3
};

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
    CopySrc = 1,
    CopyDst = 1 << 1,
    TextureBinding = 1 << 2,
    StorageBinding = 1 << 3,
    RenderAttachment = 1 << 4,
    TransientAttachment = 1 << 5
};

enum class TextureViewDimension {
    D1,
    D2,
    D2Array,
    Cube,
    CubeArray,
    D3
};

enum class TextureAspect {
    All,
    StencilOnly,
    DepthOnly
};

struct TextureViewDescriptor {
    TextureFormat format;
    TextureViewDimension dimension;
    TextureUsages usages = TextureUsages::None;
    TextureAspect aspect = TextureAspect::All;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevelCount;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayerCount;
};

class TextureView {
public:

private:
    struct Impl {
        ~Impl() = default;
    };
    std::unique_ptr<Impl> impl;
};

struct TextureDescriptor {
    Extent3D size;
    uint32_t mipLeverCount = 1;
    uint32_t sampleCount = 1;
    TextureDimension dimension = TextureDimension::D2;
    TextureFormat format;
    TextureUsages usages;
    std::vector<TextureFormat> viewFormats;
};


class Texture {
public:
    using TextureViewResult = Result<TextureView, Error>;
    [[nodiscard]] TextureViewResult createView(const TextureViewDescriptor& descriptor);

    const uint32_t width;
    const uint32_t height;
    const uint32_t depthOrArrayLayers;
    const uint32_t mipLevelCount;
    const uint32_t sampleCount;
    const TextureDimension dimension;
    const TextureFormat format;
    const TextureUsages usages;
private:
    struct Impl {
        ~Impl() = default;
    };
    std::unique_ptr<Impl> impl;
    explicit Texture(std::unique_ptr<Impl> impl, const TextureDescriptor& descriptor);
};

}

template<> struct utils::enumBitwiseOperations<renderium::TextureUsages> : std::true_type {};