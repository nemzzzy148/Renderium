//
// Created by Nemesis Verstraete on 30/09/2026.
//

#pragma once
#include "Adapter.h"
#include "Device.h"

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

    using DeviceResult = renderium::Result<std::unique_ptr<renderium::Device::Impl>, renderium::Error>;
    DeviceResult requestDevice(const renderium::DeviceDescriptor &descriptor) override {
        auto result = adapter.requestDevice(descriptor);
        if (!result.isOk()) {
            return DeviceResult::err(renderium::Error::RequestDeviceError);
        }
        return DeviceResult::ok(
            std::unique_ptr<renderium::Device::Impl>(new DeviceImpl<Api>(std::move(result.unwrap()))));
    }
private:
    explicit AdapterImpl(Adapter adapter) : adapter(std::move(adapter)) {}
    Adapter adapter;

    friend class InstanceImpl<Api>;
};

}
