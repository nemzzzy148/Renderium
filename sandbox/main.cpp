
#include <assert.h>
#include <iomanip>
#include <iostream>

#include "Renderium.h"

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
        struct VertexOutput
{
    float4 position : SV_Position;
    float3 color    : COLOR0;
};

// Vertex shader
[shader("vertex")]
VertexOutput vertMain(uint vertexID : SV_VertexID)
{
    // Hardcoded triangle vertices
    float2 positions[3] =
    {
        float2( 0.0,  0.5),
        float2( 0.5, -0.5),
        float2(-0.5, -0.5)
    };

    float3 colors[3] =
    {
        float3(1.0, 0.0, 0.0), // Red
        float3(0.0, 1.0, 0.0), // Green
        float3(0.0, 0.0, 1.0)  // Blue
    };

    VertexOutput output;

    output.position = float4(positions[vertexID], 0.0, 1.0);
    output.color = colors[vertexID];

    return output;
}

// Fragment shader
[shader("fragment")]
float4 fragMain(VertexOutput input) : SV_Target0
{
    return float4(input.color, 1.0);
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

    // adapter
    auto adapterResult = instance.requestAdapter({.compatibleSurface = &surface, .powerPreference = renderium::PowerPreference::None});
    assert(adapterResult.isOk());
    const auto adapter = adapterResult.unwrap();

    // device
    auto deviceResult = adapter.requestDevice({});
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
    auto shaderResult = device.createShaderModule({.code = slangShaderCode});
    assert(shaderResult.isOk());
    const auto shader = shaderResult.unwrap();

    // pipeline
    auto renderPipelineResult = device.createRenderPipeline({
        .vertex = {
            .module = shader,
            .entryPoint = "vertMain"
        },
        .fragment = {
            .module = shader,
            .entryPoint = "fragMain",
            .targets = {{formats[0]}}
        },
        .primitive = {
            .topology = renderium::PrimitiveTopology::TriangleList,
            .frontFace = renderium::FrontFace::CCW,
            .cullMode = renderium::CullMode::None
        }
    });
    assert(renderPipelineResult.isOk());
    const auto renderPipeline = renderPipelineResult.unwrap();

    std::cout << "pipeline created\n";
    return 0;

    // main loop
    while (!window.shouldClose()) {
        window.waitEvents();
    }
    return 0;
}