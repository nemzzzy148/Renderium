//
// Created by Nemesis Verstraete on 04/11/2026.
//

#pragma once
#include "Result.h"
#include "Texture.h"

namespace rhi {

class ITexture {
public:
    virtual ~ITexture() = default;

    virtual std::unique_ptr<renderium::TextureView::Impl>
        createView(const renderium::TextureViewDescriptor& descriptor) = 0;
};

}
