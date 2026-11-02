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

#define RETURN_INVALID_UNIQUE(T) return std::unique_ptr<renderium::T::Impl>(new T##Impl<Api>());
#define RETURN_INVALID_SHARED(T) return std::shared_ptr<renderium::T::Impl>(new T##Impl<Api>());

namespace rhi {

template<typename Api>
DeviceImpl<Api>::QueueResult DeviceImpl<Api>::createQueue() {
    auto result = Api::Queue::create(*device);
    if (!result.isOk()) {
        return QueueResult::err(renderium::RequestError::QueueCreateError);
    }
    return QueueResult::ok(std::unique_ptr<renderium::Queue::Impl>(
        new QueueImpl<Api>(std::move(result.unwrap()))));
}

template<typename Api>
void DeviceImpl<Api>::pushErrorScope(const renderium::ErrorFilter filter) {
    errorState.pushErrorScope(filter);
}

template<typename Api>
std::future<std::optional<renderium::Error>> DeviceImpl<Api>::popErrorScope() {
    return errorState.popErrorScope();
}

template<typename Api>
renderium::shader::CompiledShader DeviceImpl<Api>::compileShader(const std::string &code) {
    auto process = Process(errorState);
    auto result = shaderCompiler->compileShader(process, code);
    if (!result.isOk()) {
        return {};
    }
    return result.unwrap();
}

template<typename Api>
std::unique_ptr<renderium::RenderPipeline::Impl> DeviceImpl<Api>::createRenderPipeline(
    const renderium::RenderPipelineDescriptor &descriptor) {
    auto process = Process(errorState);
    validateRenderPipelineDescriptor(process, descriptor); // captures faults

    if (process.caughtError()) RETURN_INVALID_UNIQUE(RenderPipeline);

    auto result = Api::RenderPipeline::create(*device, descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        RETURN_INVALID_UNIQUE(RenderPipeline);
    }
    return std::unique_ptr<renderium::RenderPipeline::Impl>(
        new RenderPipelineImpl<Api>(std::move(result.unwrap())));
}

template<typename Api>
std::unique_ptr<renderium::Buffer::Impl> DeviceImpl<Api>::createBuffer(const renderium::BufferDescriptor &descriptor) {
    auto process = Process(errorState);
    validateBufferDescriptor(process, descriptor);

    if (process.caughtError()) RETURN_INVALID_UNIQUE(Buffer);

    auto result = device->createBuffer(descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        RETURN_INVALID_UNIQUE(Buffer);
    }

    return std::unique_ptr<renderium::Buffer::Impl>(
        new BufferImpl<Api>(std::move(result.unwrap()), descriptor));
}

template<typename Api>
std::unique_ptr<renderium::Texture::Impl> DeviceImpl<Api>::createTexture(
        const renderium::TextureDescriptor &descriptor) {
    auto process = Process(errorState);
    validateTextureDescriptor(process, descriptor);

    if (process.caughtError()) RETURN_INVALID_UNIQUE(Texture);

    auto result = device->createTexture(descriptor);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        RETURN_INVALID_UNIQUE(Texture);
    }

    return std::unique_ptr<renderium::Texture::Impl>(
        new TextureImpl<Api>(std::move(result.unwrap()), descriptor));
}

template<typename Api>
std::unique_ptr<renderium::CommandEncoder::Impl> DeviceImpl<Api>::createCommandEncoder() {
    auto result = device->createCommandEncoder();

    auto process = Process(errorState);
    if (!result.isOk()) {
        process.report(Api::toError(result.unwrapError()));
        RETURN_INVALID_UNIQUE(CommandEncoder);
    }

    return std::unique_ptr<renderium::CommandEncoder::Impl>(
        new CommandEncoderImpl<Api>(std::move(result.unwrap())));
}

template class DeviceImpl<VulkanApi>;

}