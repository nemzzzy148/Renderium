//
// Created by Nemesis Verstraete on 13/11/2026.
//

#pragma once
#include <map>
#include <unordered_map>
#include <vector>
#include "ResourceManager.h"


namespace resource {
struct ResourceId;
template<typename T>
class ResourceManager;

class ResourceTracker {
public:
    void addDependencies(const std::vector<ResourceId>& ids, uint32_t timelineIndex);

    template<typename T>
    void removeDependencies(ResourceManager<T>& resourceManager, uint32_t currentTimelineIndex);
private:
    uint32_t timeline = 0;
    struct TimelineFrame {
        uint32_t timelineIndex;
        std::unique_ptr<std::vector<ResourceId>> dependencies;

        bool operator>(const TimelineFrame& frame) const {
            return timelineIndex > frame.timelineIndex;
        }
    };
    void insertFrame(uint32_t timelineIndex);
    std::priority_queue<TimelineFrame, std::vector<TimelineFrame>, std::greater<TimelineFrame>> dependencies;
    std::unordered_set<uint32_t> timelineIndices;
};

}
