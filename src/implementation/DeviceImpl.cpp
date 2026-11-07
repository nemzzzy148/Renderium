//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "DeviceImpl.h"

#include "API.h"
#include "vulkan/VulkanBuffer.h"
#include "vulkan/VulkanDevice.h"
#include "vulkan/VulkanPipeline.h"
#include "vulkan/VulkanQueue.h"
#include "vulkan/VulkanTexture.h"
#include "QueueImpl.h"
#include "CommandEncoderImpl.h"
#include "../resource/ResourceManager.h"

#define RETURN_INVALID_UNIQUE(T) return std::unique_ptr<renderium::T::Impl>(new T##Impl<Api>());
#define RETURN_INVALID_SHARED(T) return std::shared_ptr<renderium::T::Impl>(new T##Impl<Api>());

namespace rhi {

template<typename Api>
DeviceImpl<Api>::QueueResult DeviceImpl<Api>::createQueue() {
    auto result = Api::Queue::create(*deviceState->device);
    if (!result.isOk()) {
        return QueueResult::err(renderium::RequestError::QueueCreateError);
    }
    return QueueResult::ok(std::shared_ptr<renderium::Queue::Impl>(
        new QueueImpl<Api>(std::move(result.unwrap()), deviceState) ));
}

template<typename Api>
void DeviceImpl<Api>::pushErrorScope(const renderium::ErrorFilter filter) {
    deviceState->errorState.pushErrorScope(filter);
}

template<typename Api>
std::future<std::optional<renderium::Error>> DeviceImpl<Api>::popErrorScope() {
    return deviceState->errorState.popErrorScope();
}

template<typename Api>
renderium::shader::CompiledShader DeviceImpl<Api>::compileShader(const std::string &code) {
    auto process = Process(deviceState->errorState);
    auto result = shaderCompiler->compileShader(process, code);
    if (!result.isOk()) {
        return {};
    }
    return result.unwrap();
}

template<typename Api>
DeviceImpl<Api>::SharedResource DeviceImpl<Api>::createRenderPipeline(
    const renderium::RenderPipelineDescriptor &descriptor) {
    auto process = Process(deviceState->errorState);
    validateRenderPipelineDescriptor(process, descriptor); // captures faults

    if (process.caughtError()) return nullptr;

    auto result = Api::RenderPipeline::create(*deviceState->device, descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        return nullptr;
    }
    auto id = deviceState->renderPipelines.allocate(std::unique_ptr<IRenderPipeline>(
        new RenderPipelineImpl<Api>(std::move(result.unwrap()))));
    return std::shared_ptr<renderium::detail::Resource>(new RenderPipelineResource<Api>(deviceState, id));
}

template<typename Api>
DeviceImpl<Api>::SharedResource DeviceImpl<Api>::createBuffer(const renderium::BufferDescriptor &descriptor) {
    auto process = Process(deviceState->errorState);
    validateBufferDescriptor(process, descriptor);

    if (process.caughtError()) return nullptr;

    auto result = deviceState->device->createBuffer(descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        return nullptr;
    }

    auto id = deviceState->buffers.allocate(std::unique_ptr<IBuffer>(
        new BufferImpl<Api>(std::move(result.unwrap()))));
    return std::shared_ptr<renderium::detail::Resource>(new BufferResource<Api>(deviceState, id));
}

template<typename Api>
DeviceImpl<Api>::SharedTextureResource DeviceImpl<Api>::createTexture(
        const renderium::TextureDescriptor &descriptor) {
    auto process = Process(deviceState->errorState);
    validateTextureDescriptor(process, descriptor);

    if (process.caughtError()) return nullptr;

    auto result = deviceState->device->createTexture(descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        return nullptr;
    }

    auto id = deviceState->textures.allocate(std::unique_ptr<ITexture>(
        new TextureImpl<Api>(std::move(result.unwrap()), deviceState)));
    return std::shared_ptr<renderium::detail::TextureResource>(new TextureResource<Api>(deviceState, id));
}

template<typename Api>
std::unique_ptr<renderium::CommandEncoder::Impl> DeviceImpl<Api>::createCommandEncoder() {
    auto result = deviceState->device->createCommandEncoder();

    auto process = Process(deviceState->errorState);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        return nullptr;
    }

    return std::unique_ptr<renderium::CommandEncoder::Impl>(
        new CommandEncoderImpl<Api>(deviceState, std::move(result.unwrap())));
}

template class DeviceImpl<VulkanApi>;

}