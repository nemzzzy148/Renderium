//
// Created by Nemesis Verstraete on 06/11/2026.
//

#pragma once
#include <unordered_map>
#include <vector>
#include <map>

#include "ScopeTracker.h"
#include "../resource/ResourceManager.h"

namespace track {

template<Usage T>
class UsageTracker {
public:
    explicit UsageTracker(uint32_t reservedSize = 1024);
    void add(resource::ResourceId id, T usage);
    void merge(const ScopeTracker<T>& scope); // return value underlines changes
    std::optional<T> get(resource::ResourceId id);
    void remove(resource::ResourceId id);
private:
    std::mutex lock;

    std::unordered_map<resource::ResourceId, T> usages;
};

}
