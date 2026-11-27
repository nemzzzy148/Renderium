
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>

#include <Renderium.h>

#include "Engine.h"

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



using namespace renderium;

int main() {
    Engine::LoopFunction loop = [] (Engine& engine) {
        const auto [surfaceTexture] = engine.getSurface().getCurrentTexture();
        const auto surfaceView = surfaceTexture.createView(TextureViewDescriptor{});

        RenderPassColorAttachment colorAttachment{
            .textureView = surfaceView,
            .clearValue = Color::Black,
            .loadOp = LoadOp::Clear,
            .storeOp = StoreOp::Store
        };


        // command encoder
        const auto commandEncoder = engine.getDevice().createCommandEncoder();

        // render pass
        const auto renderPass = commandEncoder.beginRenderPass({
            .colorAttachments = {colorAttachment}
        });

        renderPass.setPipeline(engine.getRenderPipeline());

        renderPass.draw(3);

        renderPass.end();

        const auto commandBuffer = commandEncoder.finish();

        engine.getDevice().queue().submit(commandBuffer);

        engine.getSurface().present();
    };


    auto engineResult = Engine::create();
    assert(engineResult.isOk());
    auto engine = engineResult.unwrap();
    // engine->setLoopFunction(loop); will crash when set
    engine->startLoop();

    return 0;
}