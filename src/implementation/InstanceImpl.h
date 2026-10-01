//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <Error.h>
#include <memory>

#include "Adapter.h"
#include "AdapterImpl.h"
#include "Instance.h"
#include "Result.h"
#include "DeviceImpl.h"
#include "SurfaceImpl.h"

namespace renderium {
struct InstanceDescriptor;
}

namespace rhi {
template<typename Api>
class InstanceImpl : public renderium::Instance::Impl {
public:
    using Instance = Api::Instance;
    using Error = Api::Error;

    using InstanceResult = renderium::Result<std::unique_ptr<InstanceImpl>, Error>;
    static InstanceResult create(const renderium::InstanceDescriptor& descriptor) {
        auto result = Instance::create(descriptor);
        if (!result.isOk()) {
            return InstanceResult::err(std::move(result.unwrapError()));
        }
        return InstanceResult::ok(std::unique_ptr<InstanceImpl>(new InstanceImpl(std::move(result.unwrap()))));
    }

    using AdapterResult = renderium::Result<std::unique_ptr<renderium::Adapter::Impl>, renderium::Error>;
    AdapterResult requestAdapter(const renderium::RequestAdapterOptions& options) override {
        AdapterImplOptions<Api> adapterOptions {
            .surface = options.compatibleSurface ?
                &static_cast<const SurfaceImpl<Api>&>(*options.compatibleSurface->impl).surface : nullptr,
            .powerPreference = options.powerPreference
        };
        auto result = Api::Adapter::create(instance, adapterOptions);
        if (!result.isOk()) {
            return AdapterResult::err(renderium::Error::RequestAdapterError);
        }
        return AdapterResult::ok(
            std::unique_ptr<renderium::Adapter::Impl>(new AdapterImpl<Api>(std::move(result.unwrap()))));
    }

    using SurfaceResult = renderium::Result<std::unique_ptr<renderium::Surface::Impl>, renderium::Error>;
    SurfaceResult createSurface(const renderium::Window& window) override {
        auto result = Api::Surface::create(instance, window);
        if (!result.isOk()) {
            return SurfaceResult::err(renderium::Error::SurfaceCreateError);
        }
        return SurfaceResult::ok(
            std::unique_ptr<renderium::Surface::Impl>(new SurfaceImpl<Api>(std::move(result.unwrap()))));
    }
private:
    explicit InstanceImpl(Instance instance) : instance(std::move(instance)) {}
    Instance instance;
};

}