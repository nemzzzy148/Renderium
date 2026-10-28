//
// Created by Nemesis Verstraete on 01/10/2026.
//

#include "Adapter.h"

namespace renderium {

Adapter::DeviceResult Adapter::requestDevice(const DeviceDescriptor &descriptor) const {
    auto deviceResult = impl->requestDevice(descriptor);
    if (!deviceResult.isOk()) {
        return DeviceResult::err(deviceResult.unwrapError());
    }
    auto device = deviceResult.unwrap();

    auto queueResult = device->createQueue();
    if (!queueResult.isOk()) {
        return DeviceResult::err(queueResult.unwrapError());
    }

    return DeviceResult::ok(Device(
        std::move(device),
        std::move(queueResult.unwrap()),
        features, limits, info
    ));
}

}