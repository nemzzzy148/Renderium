//
// Created by Nemesis Verstraete on 16/07/2026.
//

#pragma once
#include <memory>
#include <vector>

#include "Result.h"
#include "Resource.h"
#include "../src/utils/BitwiseOperations.h"

namespace rhi {
class ITexture;
template<typename Api>
class TextureImpl;
template<typename Api>
class TextureViewImpl;
template<typename Api>
class TextureResource;
template<typename Api>
class RenderPassImpl;
}

namespace renderium {
namespace detail {
class TextureResource;
}

enum class RequestError;

struct Extent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
};

enum class TextureDimension  {
    D1,
    D2,
    D3
};

enum class TextureFormat {
    None,

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
    TextureFormat format = TextureFormat::None;
    TextureViewDimension dimension = TextureViewDimension::D2;
    TextureUsages usage = TextureUsages::None;
    TextureAspect aspect = TextureAspect::All;
    uint32_t baseMipLevel = 0;
    uint32_t mipLevelCount = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayerCount = 1;
};

namespace detail {
class TextureResource;
}

class TextureView {
    struct Impl {
        ~Impl() = default;
    };
    explicit TextureView(std::unique_ptr<Impl> impl, std::shared_ptr<detail::TextureResource> textureResource)
        : impl(std::move(impl)), textureResource(std::move(textureResource)) {}
    std::shared_ptr<detail::TextureResource> textureResource;
    std::unique_ptr<Impl> impl;

    friend class Texture;
    friend class detail::TextureResource;
    friend class rhi::ITexture;
    template<typename Api>
    friend class rhi::TextureViewImpl;
    template<typename Api>
    friend class rhi::TextureImpl;
    template<typename Api>
    friend class rhi::TextureResource;
    template<typename Api>
    friend class rhi::RenderPassImpl;
};

namespace detail {

class TextureResource : public Resource {
public:
    virtual std::unique_ptr<TextureView::Impl> createView(const TextureViewDescriptor& descriptor) = 0;
};

}

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
    [[nodiscard]] uint32_t getWidth() const { return width; }
    [[nodiscard]] uint32_t getHeight() const { return height; }
    [[nodiscard]] uint32_t getDepthOrArrayLayers() const { return depthOrArrayLayers; }
    [[nodiscard]] uint32_t getMipLevelCount() const { return mipLevelCount; }
    [[nodiscard]] uint32_t getSampleCount() const { return sampleCount; }
    [[nodiscard]] TextureDimension getDimension() const { return dimension; }
    [[nodiscard]] TextureFormat getFormat() const { return format; }
    [[nodiscard]] TextureUsages getUsage() const { return usage; }

    [[nodiscard]] TextureView createView(const TextureViewDescriptor& descriptor) const;
private:
    explicit Texture(std::shared_ptr<detail::TextureResource> resource, const TextureDescriptor& descriptor);

    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
    uint32_t mipLevelCount;
    uint32_t sampleCount;
    TextureDimension dimension;
    TextureFormat format;
    TextureUsages usage;
    std::shared_ptr<detail::TextureResource> resource;
    friend class Device;
    friend class Surface;
};

}

template<> struct utils::enumBitwiseOperations<renderium::TextureUsages> : std::true_type {};