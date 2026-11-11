
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>

#include <Renderium.h>

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
    std::function errorCallback = [](const renderium::Error& error) {
        std::printf("%s", error.message.c_str());
    };

    // window
    auto windowResult = renderium::Window::create({});
    assert(windowResult.isOk());
    const auto window = windowResult.unwrap();
    std::printf("Window creation succeeded!\n");

    // instance
    auto instanceResult = renderium::Instance::create({});
    assert(instanceResult.isOk());
    const auto instance = instanceResult.unwrap();
    std::printf("Instance creation succeeded!\n");

    // surface
    auto surfaceResult = instance.createSurface(window);
    assert(surfaceResult.isOk());
    const auto surface = surfaceResult.unwrap();
    std::printf("Surface creation succeeded!\n");

    // adapter
    auto adapterResult = instance.requestAdapter({
        .compatibleSurface = &surface,
        .powerPreference = renderium::PowerPreference::None
    });
    assert(adapterResult.isOk());
    const auto adapter = adapterResult.unwrap();
    std::printf("Adapter creation succeeded!\n");

    // device
    auto deviceResult = adapter.requestDevice({});
    assert(deviceResult.isOk());
    const auto device = deviceResult.unwrap();
    std::printf("Device creation succeeded!\n");

    // example texture and buffer
    const auto buffer = device.createBuffer({1, renderium::BufferUsages::Vertex, false});
    std::printf("Buffer creation succeeded!\n");

    const auto texture = device.createTexture({{512, 512, 1}, 1, 1,
        renderium::TextureDimension::D2, renderium::TextureFormat::RGBA8Unorm,
        renderium::TextureUsages::StorageBinding, {renderium::TextureFormat::RGBA8Unorm}});
    std::printf("Texture creation succeeded!\n");

    const auto textureView = texture.createView(renderium::TextureViewDescriptor{}); // default values
    std::printf("Texture view creation succeeded!\n");

    // queue
    const auto queue = device.queue();
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
    const auto shaderModule = device.createShaderModule({.code = slangShaderCode}); // can be dead
    std::printf("Shader module creation succeeded!\n");

    // pipeline // seg fault with pipeline, shader is invalid
    const auto renderPipeline = device.createRenderPipeline({ // can be dead
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
            .topology = renderium::PrimitiveTopology::TriangleList,
            .frontFace = renderium::FrontFace::CCW,
            .cullMode = renderium::CullMode::None
        }
    });
    std::printf("Render pipeline creation succeeded!\n");


    // main loop
    uint32_t i = 0;
    while (!window.shouldClose()) {
        window.waitEvents();

        printf("Frame: %i\n", i++);

        // asset management is still lacking
        const auto [surfaceTexture] = surface.getCurrentTexture();
        const auto surfaceView = surfaceTexture.createView(renderium::TextureViewDescriptor{});

        renderium::RenderPassColorAttachment colorAttachment{
            .textureView = surfaceView,
            .clearValue = renderium::Color::Black,
            .loadOp = renderium::LoadOp::Clear,
            .storeOp = renderium::StoreOp::Store
        };


        // command encoder
        const auto commandEncoder = device.createCommandEncoder();

        // render pass
        const auto renderPass = commandEncoder.beginRenderPass({
            .colorAttachments = {colorAttachment}
        });

        renderPass.setPipeline(renderPipeline);

        renderPass.draw(3);

        renderPass.end();

        auto commandBuffer = commandEncoder.finish();


        // pass set pipeline
        // pass draw 3
        // pass end
        // queue submit
    }

    return 0;
}