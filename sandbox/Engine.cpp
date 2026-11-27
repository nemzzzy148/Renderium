//
// Created by Nemesis Verstraete on 27/11/2026.
//

#include "Engine.h"

#include <assert.h>

#include "Shader.h"

Engine::EngineResult Engine::create() {
    // window
    auto windowResult = Window::create({});
    if (!windowResult.isOk()) {
        return EngineResult::err(RequestError::UnhandledException);
    }
    auto window = windowResult.unwrap();
    std::printf("Window creation succeeded!\n");

    // instance
    auto instanceResult = Instance::create({});
    if (!instanceResult.isOk()) {
        return EngineResult::err(instanceResult.unwrapError());
    }
    auto instance = instanceResult.unwrap();
    std::printf("Instance creation succeeded!\n");

    // surface
    auto surfaceResult = instance.createSurface(window);
    if (!surfaceResult.isOk()) {
        return EngineResult::err(surfaceResult.unwrapError());
    }
    auto surface = surfaceResult.unwrap();
    std::printf("Surface creation succeeded!\n");

    // adapter
    auto adapterResult = instance.requestAdapter({
        .compatibleSurface = &surface,
        .powerPreference = PowerPreference::None
    });
    if (!adapterResult.isOk()) {
        return EngineResult::err(adapterResult.unwrapError());
    }
    const auto adapter = adapterResult.unwrap();
    std::printf("Adapter creation succeeded!\n");

    // device
    auto deviceResult = adapter.requestDevice({});
    if (!deviceResult.isOk()) {
        return EngineResult::err(deviceResult.unwrapError());
    }
    auto device = deviceResult.unwrap();
    std::printf("Device creation succeeded!\n");

    // example texture and buffer
    const auto buffer = device.createBuffer({1, BufferUsages::Vertex, false});
    std::printf("Buffer creation succeeded!\n");

    const auto texture = device.createTexture({{512, 512, 1}, 1, 1,
        TextureDimension::D2, TextureFormat::RGBA8Unorm,
        TextureUsages::StorageBinding, {TextureFormat::RGBA8Unorm}});
    std::printf("Texture creation succeeded!\n");

    const auto textureView = texture.createView(TextureViewDescriptor{}); // default values
    std::printf("Texture view creation succeeded!\n");

    // queue
    auto queue = device.queue();
    std::printf("Queue creation succeeded!\n");

    // surface configurations
    const auto [usages, formats, presentModes] =
        surface.getCapabilities(device);
    surface.configure({
        .device = device,
        .width = 800,
        .height = 600,
        .usage = usages,
        .format = formats[0],
        .presentMode = presentModes[0]
    });
    std::printf("Surface configuration succeeded!\n");

    // shader
    auto shaderModule = device.createShaderModule({.code = slangShaderCode}); // can be dead
    std::printf("Shader module creation succeeded!\n");

    // pipeline // seg fault with pipeline, shader is invalid
    auto renderPipeline = device.createRenderPipeline({ // can be dead
        .vertex = {
            .module = shaderModule,
            .entryPoint = "vertMain"
        },
        .fragment = {
            .module = shaderModule,
            .entryPoint = "fragMain",
            .targets = {{formats[0]}}
        },
        .primitive = {
            .topology = PrimitiveTopology::TriangleList,
            .frontFace = FrontFace::CCW,
            .cullMode = CullMode::None
        }
    });
    std::printf("Render pipeline creation succeeded!\n");

    return EngineResult::ok(std::unique_ptr<Engine>(new Engine(std::move(window), std::move(instance), std::move(surface),
        std::move(device), std::move(shaderModule), std::move(renderPipeline), std::move(queue))));
}

void Engine::setLoopFunction(LoopFunction loop) {
    loopFunc = std::move(loop);
}

void Engine::startLoop() {
    while (!window.shouldClose()) {
        window.pollEvents();
        if (!loopFunc) continue;
        loopFunc(*this);
    }
}
