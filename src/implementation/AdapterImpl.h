//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once
#include "Adapter.h"
#include "DeviceImpl.h"

namespace rhi {

template<typename Api>
struct AdapterImplOptions {
    const Api::Surface* surface;
    renderium::PowerPreference powerPreference;
};

template<typename Api>
class AdapterImpl : public renderium::Adapter::Impl {
public:
    using Adapter = Api::Adapter;

    using DeviceResult = renderium::Result<std::unique_ptr<renderium::Device::Impl>, renderium::RequestError>;
    DeviceResult requestDevice(const renderium::DeviceDescriptor &descriptor) override {
        auto deviceResult = adapter.requestDevice(descriptor);
        if (!deviceResult.isOk()) {
            return DeviceResult::err(renderium::RequestError::RequestDeviceError);
        }

        auto shaderCompilerResult = IShaderCompiler::create(
            descriptor.shadingLanguage, Api::shaderOutputLanguage);
        if (!shaderCompilerResult.isOk()) {
            return DeviceResult::err(shaderCompilerResult.unwrapError());
        }

        return DeviceResult::ok(
            std::unique_ptr<renderium::Device::Impl>(new DeviceImpl<Api>(
                std::move(deviceResult.unwrap()), std::move(shaderCompilerResult.unwrap()))));
    }

    renderium::SupportedFeatures requestFeatures() override { return adapter->requestFeatures(); }
    renderium::SupportedLimits requestLimits() override { return adapter->requestLimits(); }
    renderium::AdapterInfo requestAdapterInfo() override { return adapter->requestAdapterInfo(); }
private:
    explicit AdapterImpl(Adapter adapter) : adapter(std::move(adapter)) {}
    Adapter adapter;

    friend class InstanceImpl<Api>;
};

}
