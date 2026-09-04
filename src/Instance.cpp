//
// Created by Nemesis Verstraete on 17/08/2026.
//

#include "../include/Instance.h"

#include <memory>

#include "Error.h"
#include "implementation/InstanceImpl.h"
#include "implementation/vulkan/VulkanInstance.h"

namespace renderium {

Instance::InstanceResult Instance::create(const InstanceCreateInfo &createInfo) {
    switch (createInfo.backend) {
        case Backend::Vulkan: {
            auto result = rhi::InstanceImpl<rhi::VulkanApi>::create(createInfo);
            if (!result.isOk()) {
                return InstanceResult::err(Error::InstanceCreateError);
            }
            return InstanceResult::ok(Instance(result.unwrap()));
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

Instance::DeviceResult Instance::createDevice(const DeviceCreateInfo &createInfo) const {
    auto deviceResult = impl->createDevice(createInfo);
    if (!deviceResult.isOk()) {
        return DeviceResult::err(deviceResult.unwrapError());
    }
    auto device = deviceResult.unwrap();

    auto queueResult = device->createQueue();
    if (!queueResult.isOk()) {
        return DeviceResult::err(queueResult.unwrapError());
    }
    return DeviceResult::ok(Device(std::move(device), std::move(queueResult.unwrap())));
}

}