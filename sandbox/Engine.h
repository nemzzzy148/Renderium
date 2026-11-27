//
// Created by Nemesis Verstraete on 27/11/2026.
//

#pragma once
#include <memory>

#include "Renderium.h"

using namespace renderium;

class Engine {
public:
    using EngineResult = Result<std::unique_ptr<Engine>, RequestError>;
    static EngineResult create();

    using LoopFunction = std::function<void(Engine&)>;
    void setLoopFunction(LoopFunction loop);
    void startLoop();

    [[nodiscard]] Window& getWindow() { return window; }
    [[nodiscard]] Instance& getInstance() { return instance; }
    [[nodiscard]] Surface& getSurface() { return surface; }
    [[nodiscard]] Device& getDevice() { return device; }
    [[nodiscard]] ShaderModule getShaderModule() { return shaderModule; }
    [[nodiscard]] RenderPipeline getRenderPipeline() { return renderPipeline; }
    [[nodiscard]] Queue& getQueue() { return queue; }
private:
    explicit Engine(Window window, Instance instance, Surface surface, Device device, ShaderModule shaderModule,
            RenderPipeline renderPipeline, Queue queue)
        : window(std::move(window)), instance(std::move(instance)), surface(std::move(surface)),
          device(std::move(device)), shaderModule(std::move(shaderModule)), renderPipeline(std::move(renderPipeline)),
          queue(std::move(queue)) {}

    LoopFunction loopFunc;

    Window window;
    Instance instance;
    Surface surface;
    Device device;
    ShaderModule shaderModule;
    RenderPipeline renderPipeline;
    Queue queue;
};
