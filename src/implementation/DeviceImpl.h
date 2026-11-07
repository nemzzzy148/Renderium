//
// Created by Nemesis Verstraete on 18/08/2026.
//

#pragma once

#include "Device.h"
#include "BufferImpl.h"
#include "CommandEncoder.h"
#include "DeviceErrorState.h"
#include "IBuffer.h"
#include "PipelineImpl.h"
#include "ShaderCompilerImpl.h"
#include "TextureImpl.h"
#include "DeviceState.h"

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

    [[nodiscard]] const Device& getBackendDevice() const { return *deviceState->device; }
    [[nodiscard]] DeviceErrorState& getDeviceErrorState() { return deviceState->errorState; }

    using QueueResult = renderium::Result<std::shared_ptr<renderium::Queue::Impl>, renderium::RequestError>;
    QueueResult createQueue() override;

    void pushErrorScope(renderium::ErrorFilter filter) override;
    std::future<std::optional<renderium::Error>> popErrorScope() override;

    renderium::shader::CompiledShader compileShader(const std::string &code) override;

    SharedResource createRenderPipeline(const renderium::RenderPipelineDescriptor& descriptor) override;
    SharedResource createBuffer(const renderium::BufferDescriptor &descriptor) override;
    SharedTextureResource createTexture(const renderium::TextureDescriptor &descriptor) override;

    std::unique_ptr<renderium::CommandEncoder::Impl> createCommandEncoder() override;

private:
    explicit DeviceImpl(std::shared_ptr<DeviceState<Api>> deviceState, std::unique_ptr<IShaderCompiler> shaderCompiler)
        : shaderCompiler(std::move(shaderCompiler)), deviceState(deviceState) {}

    std::unique_ptr<IShaderCompiler> shaderCompiler;
    std::shared_ptr<DeviceState<Api>> deviceState;
    std::weak_ptr<QueueImpl<Api>> weakQueue;

    friend class SurfaceImpl<Api>;
    friend class AdapterImpl<Api>;
};

}