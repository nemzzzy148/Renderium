//
// Created by Nemesis Verstraete on 06/11/2026.
//

#pragma once
#include "DeviceErrorState.h"
#include "../resource/ResourceManager.h"

namespace rhi {
class IRenderPipeline;
class IBuffer;
class ITexture;

// shared state that keep essential resources alive
template<typename Api>
struct DeviceState {
    using Device = Api::Device;
    explicit DeviceState(std::shared_ptr<Device> device) : device(std::move(device)) {}

    DeviceErrorState errorState;
    std::shared_ptr<Device> device;

    resource::ResourceManager<ITexture> textures;
    resource::ResourceManager<IBuffer> buffers;
    resource::ResourceManager<IRenderPipeline> renderPipelines;
};

}
