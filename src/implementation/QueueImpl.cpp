//
// Created by Nemesis Verstraete on 06/11/2026.
//

#include "QueueImpl.h"

namespace rhi {

template<typename Api>
void QueueImpl<Api>::submit(const std::vector<std::reference_wrapper<const renderium::CommandBuffer>>& commandBuffers) {
    completedTimeline.set(devicePoll());
    incrementTimeline();
}

template<typename Api>
void QueueImpl<Api>::trackBuffers(const std::vector<resource::ResourceId> &ids) {
    bufferTracker.addDependencies(ids, submitTimeline.get());
}

template<typename Api>
void QueueImpl<Api>::trackTextures(const std::vector<resource::ResourceId> &ids) {
    textureTracker.addDependencies(ids, submitTimeline.get());
}

template<typename Api>
void QueueImpl<Api>::trackRenderPipelines(const std::vector<resource::ResourceId> &ids) {
    renderPipelineTracker.addDependencies(ids, submitTimeline.get());
}

template<typename Api>
void QueueImpl<Api>::incrementTimeline() {
    submitTimeline.increase();

    auto currentCompletedTimeline = completedTimeline.increase();
    bufferTracker.removeDependencies(deviceState->buffers, currentCompletedTimeline);
    textureTracker.removeDependencies(deviceState->textures, currentCompletedTimeline);
    renderPipelineTracker.removeDependencies(deviceState->renderPipelines, currentCompletedTimeline);
}

template<typename Api>
uint32_t QueueImpl<Api>::devicePoll() {
    return deviceState->device->poll(); // Polls current completed timeline index.
}
}
