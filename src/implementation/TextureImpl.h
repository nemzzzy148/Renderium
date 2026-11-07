//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <algorithm>
#include <optional>

#include "Texture.h"
#include "DeviceErrorState.h"
#include "DeviceImpl.h"
#include "ITexture.h"
#include "Resource.h"
#include "DeviceState.h"

namespace rhi {

template<typename Api>
class SurfaceImpl;

template<typename Api>
class TextureViewImpl : public renderium::TextureView::Impl {
public:
    using TextureView = Api::TextureView;
private:
    explicit TextureViewImpl(TextureView textureView) : textureView(std::move(textureView)) {}
    TextureView textureView;

    friend class TextureImpl<Api>;
};

template<typename Api>
class TextureResource final : public renderium::detail::TextureResource {
public:
    TextureResource(std::shared_ptr<DeviceState<Api>> deviceState, const resource::ResourceId id)
        : id(id), deviceState(std::move(deviceState)) {}

    ~TextureResource() override {
        deviceState->textures.removeRef(id.handle, id.version);
    }

    std::unique_ptr<renderium::TextureView::Impl>
        createView(const renderium::TextureViewDescriptor &descriptor) override;
private:
    resource::ResourceId id;
    std::shared_ptr<DeviceState<Api>> deviceState;
};

bool validateTextureDescriptor(Process& process, const renderium::TextureDescriptor& descriptor);

template<typename Api>
class TextureImpl : public ITexture {
public:
    using Texture = Api::Texture;
    std::unique_ptr<renderium::TextureView::Impl> createView(const renderium::TextureViewDescriptor &descriptor) override;
private:
    explicit TextureImpl(Texture texture, std::weak_ptr<DeviceState<Api>> weakDeviceState)
        : texture(std::move(texture)), weakDeviceState(std::move(weakDeviceState)) {}
    Texture texture;
    std::weak_ptr<DeviceState<Api>> weakDeviceState;

    friend class DeviceImpl<Api>;
    friend class SurfaceImpl<Api>;
};

}
