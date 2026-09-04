
#include <assert.h>

#include "Surface.h"
#include "Window.h"
#include "../src/implementation/InstanceImpl.h"

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

    // pipeline
    renderium::Vert
    

    // main loop
    while (!window.shouldClose()) {
        window.waitEvents();
    }
    return 0;
}