//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include <Error.h>
#include <memory>

#include "Adapter.h"
#include "AdapterImpl.h"
#include "Instance.h"
#include "Result.h"
#include "DeviceImpl.h"
#include "SurfaceImpl.h"

namespace renderium {
struct InstanceDescriptor;
}

namespace rhi {
template<typename Api>
class InstanceImpl : public renderium::Instance::Impl {
public:
    using Instance = Api::Instance;
    using Error = Api::Error;

    using InstanceResult = renderium::Result<std::unique_ptr<InstanceImpl>, Error>;
    static InstanceResult create(const renderium::InstanceDescriptor& descriptor);

    using AdapterResult = renderium::Result<std::unique_ptr<renderium::Adapter::Impl>, renderium::RequestError>;
    AdapterResult requestAdapter(const renderium::RequestAdapterOptions& options) override;

    using SurfaceResult = renderium::Result<std::unique_ptr<renderium::Surface::Impl>, renderium::RequestError>;
    SurfaceResult createSurface(const renderium::Window& window) override;
private:
    explicit InstanceImpl(std::shared_ptr<Instance> instance) : instance(std::move(instance)) {}
    std::shared_ptr<Instance> instance;
};

}