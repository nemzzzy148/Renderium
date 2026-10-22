//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "Result.h"
#include "../src/utils/BitwiseOperations.h"

namespace rhi {
template<typename Api>
class TextureImpl;
template<typename Api>
class DeviceImpl;
}

namespace renderium {
enum class RequestError;

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
    uint32_t mipLevelCount = 1;
    uint32_t sampleCount = 1;
    TextureDimension dimension = TextureDimension::D2;
    TextureFormat format;
    TextureUsages usage;
    std::vector<TextureFormat> viewFormats;
};


class Texture {
public:
    using TextureViewResult = Result<TextureView, RequestError>;
    [[nodiscard]] TextureViewResult createView(const TextureViewDescriptor& descriptor);

    [[nodiscard]] uint32_t width() const { return impl->width; }
    [[nodiscard]] uint32_t height() const { return impl->height; }
    [[nodiscard]] uint32_t depthOrArrayLayers() const { return impl->depthOrArrayLayers; }
    [[nodiscard]] uint32_t mipLevelCount() const { return impl->mipLevelCount; }
    [[nodiscard]] uint32_t sampleCount() const { return impl->sampleCount; }
    [[nodiscard]] TextureDimension dimension() const { return impl->dimension; }
    [[nodiscard]] TextureFormat format() const { return impl->format; }
    [[nodiscard]] TextureUsages usage() const { return impl->usage; }
private:
    struct Impl {
        ~Impl() = default;

        explicit Impl(const TextureDescriptor& descriptor): width(descriptor.size.width), height(descriptor.size.height),
            depthOrArrayLayers(descriptor.size.depthOrArrayLaters), mipLevelCount(descriptor.mipLevelCount),
            sampleCount(descriptor.sampleCount), dimension(descriptor.dimension), format(descriptor.format),
            usage(descriptor.usage) {}

        const uint32_t width;
        const uint32_t height;
        const uint32_t depthOrArrayLayers;
        const uint32_t mipLevelCount;
        const uint32_t sampleCount;
        const TextureDimension dimension;
        const TextureFormat format;
        const TextureUsages usage;
    };
    std::unique_ptr<Impl> impl;
    explicit Texture(std::unique_ptr<Impl> impl, const TextureDescriptor&)
        : impl(std::move(impl)) {}

    friend class Device;
    template<typename Api>
    friend class rhi::TextureImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}

template<> struct utils::enumBitwiseOperations<renderium::TextureUsages> : std::true_type {};