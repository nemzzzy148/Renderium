//
// Created by Nemesis Verstraete on 07/11/2026.
//

#include "InstanceImpl.h"

namespace rhi {

template<typename Api>
InstanceImpl<Api>::InstanceResult InstanceImpl<Api>::create(const renderium::InstanceDescriptor &descriptor) {
    auto result = Instance::create(descriptor);
    if (!result.isOk()) {
        return InstanceResult::err(std::move(result.unwrapError()));
    }
    return InstanceResult::ok(std::unique_ptr<InstanceImpl>(
        new InstanceImpl(std::make_shared<Instance>(std::move(result.unwrap())))));
}

template<typename Api>
InstanceImpl<Api>::AdapterResult InstanceImpl<Api>::requestAdapter(
    const renderium::RequestAdapterOptions &options) {
    AdapterImplOptions<Api> adapterOptions {
        .surface = options.compatibleSurface ?
            &static_cast<const SurfaceImpl<Api>&>(*options.compatibleSurface->impl).surface : nullptr,
        .powerPreference = options.powerPreference
    };
    auto result = Api::Adapter::create(instance, adapterOptions);
    if (!result.isOk()) {
        return AdapterResult::err(renderium::RequestError::RequestAdapterError);
    }
    return AdapterResult::ok(
        std::unique_ptr<renderium::Adapter::Impl>(new AdapterImpl<Api>(std::move(result.unwrap()))));
}

template<typename Api>
InstanceImpl<Api>::SurfaceResult InstanceImpl<Api>::createSurface(const renderium::Window &window) {
    auto result = Api::Surface::create(*instance, window);
    if (!result.isOk()) {
        return SurfaceResult::err(renderium::RequestError::SurfaceCreateError);
    }
    return SurfaceResult::ok(
        std::unique_ptr<renderium::Surface::Impl>(new SurfaceImpl<Api>(std::move(result.unwrap()))));
}

}