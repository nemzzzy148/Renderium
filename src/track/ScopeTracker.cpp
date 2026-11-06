//
// Created by Nemesis Verstraete on 06/11/2026.
//

#include "ScopeTracker.h"
#include <ranges>

namespace track {

template<Usage T>
ScopeTracker<T>::ScopeTracker(const std::vector<std::pair<resource::ResourceId, T>>& beginUsages) {
    changes.reserve(beginUsages.size());
    for (const auto& item : beginUsages) {
        changes.emplace(item.first, Merge<T>{item.second, item.second});
    }
}

template<Usage T>
void ScopeTracker<T>::setUsage(resource::ResourceId id, T usage) {
    if (!changes.contains(id)) return;
    changes.at(id).end = usage;
}

}