//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "Device.h"

#include "Queue.h"
#include "Shader.h"
#include "Error.h"

namespace renderium {

Device::ShaderResult Device::createShader(const std::string& shaderCode) const {
    auto compiledShaderResult = shaderCompiler->compileShader(shaderCode);
    if (!compiledShaderResult.isOk()) {
        return ShaderResult::err(compiledShaderResult.unwrapError());
    }
    return ShaderResult::ok(Shader(std::move(compiledShaderResult.unwrap())));
}

Queue Device::getQueue() const {
    if (!queueImpl) // normally this error is impossible
        throw std::runtime_error("Renderium fatal error: failed to get queue on device!");
    return Queue(*queueImpl);
}

}