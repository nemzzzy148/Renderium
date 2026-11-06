//
// Created by Nemesis Verstraete on 05/11/2026.
//

#include "Pipeline.h"

namespace renderium {

RenderPipeline::RenderPipeline(std::shared_ptr<detail::Resource> resource) : resource(std::move(resource)) {}

}
