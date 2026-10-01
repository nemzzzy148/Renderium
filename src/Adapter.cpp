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

    auto shaderCompilerResult = shader::ShaderCompiler::create(
        shader::DefaultShadingLanguage, shader::apiToShadingLanguage(backend));
    if (!shaderCompilerResult.isOk()) {
        return DeviceResult::err(shaderCompilerResult.unwrapError());
    }

    return DeviceResult::ok(Device(
        std::move(device),
        std::move(queueResult.unwrap()),
        std::make_unique<shader::ShaderCompiler>(std::move(shaderCompilerResult.unwrap()))));
}

}
