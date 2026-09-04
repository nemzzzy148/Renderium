//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>
#include <vector>

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
    TextureFormat format;

    PresentMode presentMode = PresentMode::VSync;
    uint32_t maxFramesInFlight = 3;
};

class Surface {
public:
    [[nodiscard]] SurfaceCapabilities getCapabilities(const Device& device) const {
        return impl->getCapabilities(device);
    }
    void configure(const SurfaceConfiguration& configuration) const {
        impl->configure(configuration);
    }
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual SurfaceCapabilities getCapabilities(const Device& device) = 0;
        virtual void configure(const SurfaceConfiguration& configuration) = 0;
    };
    explicit Surface(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}
    std::unique_ptr<Impl> impl;

    friend class Instance;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    template<typename Api>
    friend class rhi::InstanceImpl;
    template<typename Api>
    friend class rhi::DeviceImpl;
};

}
