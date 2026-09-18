
#include <assert.h>
#include <iomanip>
#include <iostream>

#include "Surface.h"
#include "Window.h"
#include "../src/implementation/InstanceImpl.h"

void printUInt8Vector(const std::vector<uint8_t>& text) {
    for (const uint8_t byte : text) {
        if (std::isprint(byte)) {
            std::cout << static_cast<char>(byte);
        } else {
            if (byte == '\n') {
                std::cout << std::endl;
            }
        }
    }
    std::cout << std::dec << std::endl;
}

const std::string slangShaderCode = R"(
        // Slang supports HLSL-style structures and attributes
        StructuredBuffer<float>   buffer0;
        StructuredBuffer<float>   buffer1;
        RWStructuredBuffer<float> result;

        [shader("compute")]
        [numthreads(1, 1, 1)]
        void computeMain(uint3 threadId : SV_DispatchThreadID)
        {
            uint index = threadId.x;
            result[index] = buffer0[index] + buffer1[index];
        }
    )";

int main() {
    // window
    auto windowResult = renderium::Window::create({});
    assert(windowResult.isOk());
    const auto window = windowResult.unwrap();

    // instance
    auto instanceResult = renderium::Instance::create({});
    assert(instanceResult.isOk());
    const auto instance = instanceResult.unwrap();

    // surface
    auto surfaceResult = instance.createSurface(window);
    assert(surfaceResult.isOk());
    const auto surface = surfaceResult.unwrap();

    // device
    auto deviceResult = instance.createDevice({.compatibleSurface = &surface});
    assert(deviceResult.isOk());
    const auto device = deviceResult.unwrap();

    // queue
    const auto queue = device.getQueue();

    // surface configurations
    auto [usages, formats, presentModes] =
        surface.getCapabilities(device);
    surface.configure({
        .device = device,
        .width = 800,
        .height = 600,
        .usage = usages,
        .format = formats[0],
        .presentMode = presentModes[0]
    });

    // shader
    auto shaderCompilerResult = renderium::shader::ShaderCompiler::create(
        renderium::shader::ShadingLanguage::SLANG, renderium::shader::ShadingOutputLanguage::MSL);
    assert(shaderCompilerResult.isOk());
    const auto shaderCompiler = shaderCompilerResult.unwrap();

    renderium::shader::ShaderCompiler::ShaderResult compiledShaderResult = shaderCompiler.compileShader(slangShaderCode);
    assert(compiledShaderResult.isOk());
    auto compiledShader = compiledShaderResult.unwrap();
    assert(!compiledShader.shaderEntryPoints.empty());

    const auto& code = compiledShader.shaderEntryPoints[0].code;
    printUInt8Vector(code);

    // pipeline

    return 0;

    // main loop
    while (!window.shouldClose()) {
        window.waitEvents();
    }
    return 0;
}