//
// Created by Nemesis Verstraete on 17/08/2026.
//

#include "../include/Instance.h"

#include <memory>

#include "Error.h"
#include "implementation/InstanceImpl.h"
#include "implementation/vulkan/VulkanAdapter.h"
#include "implementation/vulkan/VulkanInstance.h"

namespace renderium {

Instance::InstanceResult Instance::create(const InstanceDescriptor& descriptor) {
    switch (descriptor.backend) {
        case Backend::Vulkan: {
            auto result = rhi::InstanceImpl<rhi::VulkanApi>::create(descriptor);
            if (!result.isOk()) {
                return InstanceResult::err(Error::InstanceCreateError);
            }
            return InstanceResult::ok(Instance(result.unwrap(), Backend::Vulkan));
        }
    }
    return InstanceResult::err(Error::InstanceCreateError);
}

Instance::SurfaceResult Instance::createSurface(const Window& window) const {
    auto result = impl->createSurface(window);
    if (!result.isOk()) {
        return SurfaceResult::err(result.unwrapError());
    }
    return SurfaceResult::ok(Surface(result.unwrap()));
}

Instance::AdapterResult Instance::requestAdapter(const RequestAdapterOptions &options) const {
    auto result = impl->requestAdapter(options);
    if (!result.isOk()) {
        return AdapterResult::err(Error::RequestAdapterError);
    }
    return AdapterResult::ok(Adapter(std::move(result.unwrap()), backend));
}

}