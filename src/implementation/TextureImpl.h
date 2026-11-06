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
class TextureResource final : public renderium::detail::Resource {
public:
    TextureResource(std::shared_ptr<DeviceState<Api>> deviceState, const resource::ResourceId id)
        : id(id), deviceState(std::move(deviceState)) {}

    ~TextureResource() override {
        deviceState->textures.removeRef(id.handle, id.version);
    }
private:
    resource::ResourceId id;
    std::shared_ptr<DeviceState<Api>> deviceState;
};

bool validateTextureDescriptor(Process& process, const renderium::TextureDescriptor& descriptor);

template<typename Api>
class TextureImpl : public ITexture {
public:
    using Texture = Api::Texture;
private:
    explicit TextureImpl(Texture texture, const renderium::TextureDescriptor& descriptor)
        : texture(std::move(texture)) {}
    Texture texture;

    friend class DeviceImpl<Api>;
};

}
