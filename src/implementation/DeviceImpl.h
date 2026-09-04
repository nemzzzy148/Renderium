//
// Created by Nemesis Verstraete on 18/08/2026.
//

#pragma once
#include "Device.h"
#include "QueueImpl.h"

namespace renderium {
struct DeviceCreateInfo;
}

namespace rhi {

template<typename Api>
class SurfaceImpl;

template<typename Api>
struct DeviceImplCreateInfo {
    const Api::Surface* surface;
    renderium::PowerPreferences powerPreferences;
};

template<typename Api>
class DeviceImpl : public renderium::Device::Impl {
public:
    using Device = Api::Device;

    [[nodiscard]] const Device& getBackendDevice() const { return device; }

    QueueImpl<Api>::QueueResult createQueue() override {
        return QueueImpl<Api>::create(device);
    }
private:
    explicit DeviceImpl(Device device) : device(std::move(device)) {}
private:
    Device device;

    friend class SurfaceImpl<Api>;
    friend class InstanceImpl<Api>;

};

}