//
// Created by Nemesis Verstraete on 18/08/2026.
//

#pragma once
#include "Device.h"
#include "QueueImpl.h"
#include "PipelineImpl.h"
#include "vulkan/VulkanPipeline.h"

namespace shader {
struct IShaderCompiler;
}

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

    [[nodiscard]] const Device& getBackendDevice() const { return device; }

    using QueueResult = renderium::Result<std::unique_ptr<renderium::Queue::Impl>, renderium::Error>;
    QueueResult createQueue() override {
        auto result = Api::Queue::create(device);
        if (!result.isOk()) {
            return QueueResult::err(renderium::Error::QueueCreateError);
        }
        return QueueResult::ok(std::unique_ptr<renderium::Queue::Impl>(
            new QueueImpl<Api>(std::move(result.unwrap()))));
    }

    using PipelineResult = renderium::Result<std::unique_ptr<renderium::RenderPipeline::Impl>, renderium::Error>;
    renderium::Result<std::unique_ptr<renderium::RenderPipeline::Impl>, renderium::Error> createRenderPipeline(
        const renderium::RenderPipelineDescriptor& descriptor) override {
        auto result = Api::RenderPipeline::create(device, descriptor);
        if (!result.isOk()) {
            return PipelineResult::err(renderium::Error::RenderPipelineCreateError);
        }
        return PipelineResult::ok(std::unique_ptr<renderium::RenderPipeline::Impl>(
            new RenderPipelineImpl<Api>(std::move(result.unwrap()))));
    }
private:
    explicit DeviceImpl(Device device) : device(std::move(device)) {}
    Device device;

    friend class SurfaceImpl<Api>;
    friend class AdapterImpl<Api>;
};

}