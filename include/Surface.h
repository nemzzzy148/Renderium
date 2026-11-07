//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>
#include <vector>

#include "Texture.h"

namespace rhi {
template<typename Api>
class SurfaceImpl;
template<typename Api>
class InstanceImpl;
template<typename Api>
class DeviceImpl;
}

namespace obsidium {
class Window;
}

namespace renderium {
enum class TextureUsages : uint8_t;
class Device;
enum class TextureFormat;
class Instance;

enum class PresentMode {
    Immediate,
    VSync,
    Mailbox,
    Relaxed
};

struct SurfaceCapabilities {
    TextureUsages usages;
    std::vector<TextureFormat> formats;
    std::vector<PresentMode> presentModes;
};

struct SurfaceConfiguration {
    const Device& device;
    uint32_t width;
    uint32_t height;

    TextureUsages usage;
    TextureFormat format = TextureFormat::BGRA8Unorm;

    PresentMode presentMode = PresentMode::VSync;
    uint32_t maxFramesInFlight = 3;
};

struct SurfaceTexture {
    Texture texture;
};

class Surface {
public:
    [[nodiscard]] SurfaceCapabilities getCapabilities(const Device& device) const;
    void configure(const SurfaceConfiguration& configuration);
    [[nodiscard]] SurfaceTexture getCurrentTexture() const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual SurfaceCapabilities getCapabilities(const Device& device) = 0;
        virtual bool configure(const SurfaceConfiguration& configuration) = 0;

        virtual std::shared_ptr<detail::Resource> getCurrentTexture() = 0;
    };
    explicit Surface(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}

    std::unique_ptr<Impl> impl;

    TextureDescriptor currentTextureState {
        .size = {0, 0, 1},
        .mipLevelCount = 1,
        .sampleCount = 1,
        .dimension = TextureDimension::D2,
        .format = TextureFormat::BGRA8Unorm,
        .usage = TextureUsages::None,
        .viewFormats = {TextureFormat::BGRA8Unorm}
    };

    friend class Instance;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    template<typename Api>
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}
