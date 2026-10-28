//
// Created by Nemesis Verstraete on 18/08/2026.
//

#pragma once

#include "Device.h"
#include "BufferImpl.h"
#include "DeviceErrorState.h"
#include "PipelineImpl.h"
#include "ShaderCompilerImpl.h"
#include "TextureImpl.h"

namespace renderium {
struct DeviceDescriptor;
}

namespace rhi {

template<typename Api>
class SurfaceImpl;

template<typename Api>
class DeviceImpl : public renderium::Device::Impl {
public:
    using Device = Api::Device;

    [[nodiscard]] const Device& getBackendDevice() const { return *device; }

    using QueueResult = renderium::Result<std::unique_ptr<renderium::Queue::Impl>, renderium::RequestError>;
    QueueResult createQueue() override;

    renderium::shader::CompiledShader compileShader(const std::string &code) override;
    std::unique_ptr<renderium::RenderPipeline::Impl> createRenderPipeline(
        const renderium::RenderPipelineDescriptor& descriptor) override;
    std::unique_ptr<renderium::Buffer::Impl> createBuffer(const renderium::BufferDescriptor &descriptor) override;
    std::unique_ptr<renderium::Texture::Impl> createTexture(const renderium::TextureDescriptor &descriptor) override;

private:
    explicit DeviceImpl(Device device, std::unique_ptr<IShaderCompiler> shaderCompiler)
        : shaderCompiler(std::move(shaderCompiler)), device(std::make_shared<Device>(std::move(device))) {}
private:
    std::unique_ptr<IShaderCompiler> shaderCompiler;
    std::shared_ptr<Device> device;
    DeviceErrorState errorState;

    friend class SurfaceImpl<Api>;
    friend class AdapterImpl<Api>;
};

}