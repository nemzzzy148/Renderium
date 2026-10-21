//
// Created by Nemesis Verstraete on 18/08/2026.
//

#pragma once
#include <stack>

#include "Device.h"
#include "BufferImpl.h"
#include "QueueImpl.h"
#include "PipelineImpl.h"
#include "TextureImpl.h"
#include "vulkan/VulkanPipeline.h"

#define RETURN_INVALID_UNIQUE(T) return std::unique_ptr<renderium::T::Impl>(T##Impl<Api>());

namespace shader {
struct IShaderCompiler;
}

namespace renderium {
struct DeviceDescriptor;
}

namespace rhi {

class DeviceErrorState {
public:
    void pushErrorScope(renderium::ErrorFilter type);
    void report(renderium::Error error);
    renderium::Error popErrorScope();
private:
    std::stack<renderium::Error> errorStack;
};

template<typename Api>
class SurfaceImpl;

template<typename Api>
class DeviceImpl : public renderium::Device::Impl {
public:
    using Device = Api::Device;

    [[nodiscard]] const Device& getBackendDevice() const { return *device; }

    using QueueResult = renderium::Result<std::unique_ptr<renderium::Queue::Impl>, renderium::RequestError>;
    QueueResult createQueue() override {
        auto result = Api::Queue::create(*device);
        if (!result.isOk()) {
            return QueueResult::err(renderium::RequestError::QueueCreateError);
        }
        return QueueResult::ok(std::unique_ptr<renderium::Queue::Impl>(
            new QueueImpl<Api>(std::move(result.unwrap()))));
    }

    std::unique_ptr<renderium::RenderPipeline::Impl> createRenderPipeline(
        const renderium::RenderPipelineDescriptor& descriptor) override {

        if (!descriptor.vertex.module.isValid()) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "The vertex shader module is not valid for creating a render pipeline."
            });
            RETURN_INVALID_UNIQUE(RenderPipeline);
        }

        if (descriptor.vertex.entryPoint.empty()) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "The vertex entry point string is empty. "
                "The string content is required to locate the entry point function for the vertex state."
            });
            RETURN_INVALID_UNIQUE(RenderPipeline);
        }

        if (!descriptor.fragment.module.isValid()) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "The fragment shader module is not valid for creating a render pipeline."
            });
            RETURN_INVALID_UNIQUE(RenderPipeline);
        }

        if (descriptor.fragment.entryPoint.empty()) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "The fragment entry point string is empty. "
                "The string content is required to locate the entry point function for the fragment state."
            });
            RETURN_INVALID_UNIQUE(RenderPipeline);
        }

        if (descriptor.fragment.targets.empty()) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "No render targets were specified when creating a render pipeline."
            });
            RETURN_INVALID_UNIQUE(RenderPipeline);
        }

        auto result = Api::RenderPipeline::create(*device, descriptor);
        if (!result.isOk()) {
            return std::unique_ptr<renderium::RenderPipeline::Impl>(
                new RenderPipelineImpl<Api>(std::move(result.unwrap())));
            // !!! add validation support !!!
        }
        return std::unique_ptr<renderium::RenderPipeline::Impl>(
            new RenderPipelineImpl<Api>(std::move(result.unwrap())));
    }

    std::unique_ptr<renderium::Buffer::Impl> createBuffer(const renderium::BufferDescriptor &descriptor) override {
        if (descriptor.size == 0) {
            errorState->report({
                renderium::ErrorFilter::Validation,
                "The size of a buffer cannot be 0."
            });
        }

        auto result = device->createBuffer(descriptor);
        if (!result.isOk()) {
            errorState->report(Api::toError(result.unwrap()));
            return  std::unique_ptr<renderium::Buffer::Impl>(BufferImpl<Api>());
        }

        return std::unique_ptr<renderium::Buffer::Impl>(
            new BufferImpl<Api>(std::move(result.unwrap()), descriptor));
    }

    std::unique_ptr<renderium::Texture::Impl> createTexture(const renderium::TextureDescriptor &descriptor) override {
        auto result = device->createTexture(descriptor);
        return std::unique_ptr<renderium::Texture::Impl>(
            new TextureImpl<Api>(std::move(result.unwrap()), descriptor));
    }

private:
    explicit DeviceImpl(Device device) : device(std::make_shared<Device>(std::move(device))) {}

    std::shared_ptr<Device> device;
    std::shared_ptr<DeviceErrorState> errorState = std::make_shared<DeviceErrorState>();

    friend class SurfaceImpl<Api>;
    friend class AdapterImpl<Api>;
};

}