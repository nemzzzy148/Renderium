//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Adapter.h"
#include "Backend.h"
#include "Device.h"
#include "Result.h"
#include "Surface.h"

namespace rhi {
template<typename Api>
class InstanceImpl;
}

namespace renderium {
enum class RequestError;
class Window;

struct InstanceDescriptor {
    Backend backend = Backend::Vulkan;
    bool debug = false;
};

class Instance {
public:
    using InstanceResult = Result<Instance, RequestError>;
    static InstanceResult create(const InstanceDescriptor& descriptor);

    using SurfaceResult = Result<Surface, RequestError>;
    [[nodiscard]] SurfaceResult createSurface(const Window& window) const;

    using AdapterResult = Result<Adapter, RequestError>;
    [[nodiscard]] AdapterResult requestAdapter(const RequestAdapterOptions& options) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::unique_ptr<Surface::Impl>, RequestError> createSurface(const Window& window) = 0;
        virtual Result<std::unique_ptr<Adapter::Impl>, RequestError> requestAdapter(const RequestAdapterOptions& options) = 0;
    };

    explicit Instance(std::unique_ptr<Impl> impl, const Backend backend)
        : impl(std::move(impl)), backend(backend) {}
    std::unique_ptr<Impl> impl;
    Backend backend;

    template<typename Api>
    friend class rhi::InstanceImpl;
};

}
