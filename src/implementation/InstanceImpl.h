//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <Error.h>
#include <memory>

#include "Instance.h"
#include "Result.h"
#include "DeviceImpl.h"
#include "ShaderCompilerImpl.h"
#include "SurfaceImpl.h"

namespace renderium {
struct InstanceCreateInfo;
}

namespace rhi {
template<typename Api>
class InstanceImpl : public renderium::Instance::Impl {
public:
    using Instance = Api::Instance;
    using Error = Api::Error;

    using InstanceResult = renderium::Result<std::unique_ptr<InstanceImpl>, Error>;
    static InstanceResult create(const renderium::InstanceCreateInfo& createInfo) {
        auto result = Instance::create(createInfo);
        if (!result.isOk()) {
            return InstanceResult::err(std::move(result.unwrapError()));
        }
        return InstanceResult::ok(std::unique_ptr<InstanceImpl>(new InstanceImpl(std::move(result.unwrap()))));
    }

    using DeviceResult = renderium::Result<std::unique_ptr<renderium::Device::Impl>, renderium::Error>;
    DeviceResult createDevice(const renderium::DeviceCreateInfo& createInfo) override {
        DeviceImplCreateInfo<Api> implCreateInfo{
            .surface = createInfo.compatibleSurface ? reinterpret_cast<const Api::Surface*>(createInfo.compatibleSurface->impl.get()) : nullptr,
            .powerPreferences = createInfo.powerPreferences
        };
        auto deviceResult = instance.createDevice(implCreateInfo);
        if (!deviceResult.isOk()) {
            return DeviceResult::err(renderium::Error::DeviceCreateError);
        }

        auto shaderCompilerResult = ShaderCompilerImpl<shader::SlangApi>::create();
        return DeviceResult::ok(std::unique_ptr<renderium::Device::Impl>(new DeviceImpl<Api>(std::move(deviceResult.unwrap()))));
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