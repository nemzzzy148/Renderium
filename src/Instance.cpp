//
// Created by Nemesis Verstraete on 17/08/2026.
//

#include "../include/Instance.h"

#include <memory>

#include "Error.h"
#include "implementation/vulkan/VulkanAdapter.h"
#include "implementation/vulkan/VulkanBuffer.h"
#include "implementation/vulkan/VulkanInstance.h"
#include "implementation/vulkan/VulkanTexture.h"
#include "implementation/InstanceImpl.h"

namespace renderium {

Instance::InstanceResult Instance::create(const InstanceDescriptor& descriptor) {
    switch (descriptor.backend) {
        case Backend::Vulkan: {
            auto result = rhi::InstanceImpl<rhi::VulkanApi>::create(descriptor);
            if (!result.isOk()) {
                return InstanceResult::err(RequestError::InstanceCreateError);
            }
            return InstanceResult::ok(Instance(result.unwrap(), Backend::Vulkan));
        }
    }
    return InstanceResult::err(RequestError::InstanceCreateError);
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
        return AdapterResult::err(RequestError::RequestAdapterError);
    }
    return AdapterResult::ok(Adapter(std::move(result.unwrap()), backend));
}

}