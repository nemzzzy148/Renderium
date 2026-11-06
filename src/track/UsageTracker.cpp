//
// Created by Nemesis Verstraete on 06/11/2026.
//

#include "UsageTracker.h"

#include <__ranges/views.h>
#include <ranges>

namespace track {

template<Usage T>
UsageTracker<T>::UsageTracker(const uint32_t reservedSize) {
    usages.reserve(reservedSize);
}

template<Usage T>
void UsageTracker<T>::add(resource::ResourceId id, T usage) {
    std::lock_guard guard(lock);
    usages.insert(id, usage);
}

template<Usage T>
void UsageTracker<T>::merge(const ScopeTracker<T>& scope) {
    std::lock_guard guard(lock);
    const auto& changes = scope.getChanges();
    for (const auto& change : changes) {
        if (change.second.begin == change.second.end) return; // no changes were made
        if (!usages.contains(change.first) || usages.at() != change.second.begin) continue;
        usages.at(change.first) = change.second.end;
    }
}

template<Usage T>
std::optional<T> UsageTracker<T>::get(resource::ResourceId id) {
    std::lock_guard guard(lock);
    if (!usages.contains(id)) return std::nullopt;
    return usages.at(id);
}

template<Usage T>
void UsageTracker<T>::remove(resource::ResourceId id) {
    std::lock_guard guard(lock);
    if (!usages.contains(id)) return;
    usages.erase(id);
}

}
