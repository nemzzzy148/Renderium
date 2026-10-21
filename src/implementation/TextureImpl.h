//
// Created by Nemesis Verstraete on 02/10/2026.
//

#pragma once
#include <algorithm>

#include "Texture.h"

namespace rhi {

template<typename Api>
class TextureImpl : public renderium::Texture::Impl {
public:
    using Texture = Api::Texture;
private:
    explicit TextureImpl(Texture texture, const renderium::TextureDescriptor& descriptor)
        : Impl(descriptor), texture(std::move(texture)) {}
    Texture texture;

    friend class DeviceImpl<Api>;
};

}
