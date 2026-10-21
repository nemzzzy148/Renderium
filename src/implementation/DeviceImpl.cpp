//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "DeviceImpl.h"

#define RETURN_INVALID_UNIQUE(T) return std::unique_ptr<renderium::T::Impl>(T##Impl<Api>());

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
std::unique_ptr<renderium::RenderPipeline::Impl> DeviceImpl<Api>::createRenderPipeline(
    const renderium::RenderPipelineDescriptor &descriptor) {
    if (validateRenderPipelineDescriptor(errorState, descriptor)) RETURN_INVALID_UNIQUE(RenderPipeline);

    auto result = Api::RenderPipeline::create(*device, descriptor);
    if (!result.isOk()) {
        errorState.report(Api::toError(result.unwrap()));
        RETURN_INVALID_UNIQUE(RenderPipeline);
    }
    return std::unique_ptr<renderium::RenderPipeline::Impl>(
        new RenderPipelineImpl<Api>(std::move(result.unwrap())));
}

template<typename Api>
std::unique_ptr<renderium::Buffer::Impl> DeviceImpl<Api>::createBuffer(const renderium::BufferDescriptor &descriptor) {

    if (validateBufferDescriptor(errorState, descriptor)) RETURN_INVALID_UNIQUE(Buffer);

    auto result = device->createBuffer(descriptor);
    if (!result.isOk()) {
        errorState.report(Api::toError(result.unwrap()));
        RETURN_INVALID_UNIQUE(Buffer);
    }

    return std::unique_ptr<renderium::Buffer::Impl>(
        new BufferImpl<Api>(std::move(result.unwrap()), descriptor));
}

template<typename Api>
std::unique_ptr<renderium::Texture::Impl> DeviceImpl<Api>::createTexture(
        const renderium::TextureDescriptor &descriptor) {
    if (validateTextureDescriptor(errorState, descriptor)) RETURN_INVALID_UNIQUE(Texture);

    auto result = device->createTexture(descriptor);
    if (!result.isOk()) {
        errorState.report(Api::toError(result.unwrap()));
        RETURN_INVALID_UNIQUE(Texture);
    }

    return std::unique_ptr<renderium::Texture::Impl>(
        new TextureImpl<Api>(std::move(result.unwrap()), descriptor));
}

}