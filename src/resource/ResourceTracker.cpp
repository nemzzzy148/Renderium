//
// Created by Nemesis Verstraete on 13/11/2026.
//

#include "ResourceTracker.h"

namespace resource {

void ResourceTracker::addDependencies(const std::vector<ResourceId> &ids, const uint32_t timelineIndex) {
    // Create list for dependencies at specific timeline index if not available.
    if (timelineIndex < timeline) return; // inserted timeline index is outdated.
    insertFrame(timelineIndex);

    auto& vec = *dependencies.top().dependencies;
    vec.reserve(vec.size() + ids.size());
    for (const auto& id : ids) {
        vec.emplace_back(id);
    }
}

void ResourceTracker::insertFrame(uint32_t timelineIndex) {
    if (timelineIndices.contains(timelineIndex)) return;
    dependencies.emplace(timelineIndex, std::make_unique<std::vector<ResourceId>>());
    timelineIndices.emplace(timelineIndex);
}

template<typename T>
void ResourceTracker::removeDependencies(ResourceManager<T> &resourceManager, const uint32_t currentTimelineIndex) {
    if (dependencies.empty()) return;

    while (!dependencies.empty() && dependencies.top().timelineIndex < currentTimelineIndex) {
        const auto& top = dependencies.top();
        for (const auto& id : *top.dependencies) {
            resourceManager.releaseResource(id);
        }
        timelineIndices.erase(top.timelineIndex);
        dependencies.pop();
    }
}

}