//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <algorithm>
#include <optional>

#include "Texture.h"
#include "DeviceErrorState.h"

namespace rhi {

bool validateTextureDescriptor(const DeviceErrorState& errorState, const renderium::TextureDescriptor& descriptor);

template<typename Api>
class TextureImpl : public renderium::Texture::Impl {
public:
    using Texture = Api::Texture;
private:
    explicit TextureImpl(Texture texture, const renderium::TextureDescriptor& descriptor)
        : Impl(descriptor), texture(std::move(texture)) {}
    explicit TextureImpl()
        : Impl(renderium::TextureDescriptor{}), texture(std::nullopt) {}
    std::optional<Texture> texture;

    friend class DeviceImpl<Api>;
};

}