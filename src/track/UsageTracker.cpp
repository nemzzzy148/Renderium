//
// Created by Nemesis Verstraete on 06/11/2026.
//

#include "UsageTracker.h"

#include "Texture.h"

#include <ranges>

namespace track {

template<Usage T>
UsageTracker<T>::UsageTracker(const uint32_t reservedSize) {
    usages.reserve(reservedSize);
}

template<Usage T>
void UsageTracker<T>::add(resource::ResourceId id) {
    std::lock_guard guard(lock);
    usages.try_emplace(id, std::nullopt);
}

template<Usage T>
void UsageTracker<T>::merge(const ScopeTracker<T>& scope) {
    std::lock_guard guard(lock);
    const auto& changes = scope.getChanges();
    for (const auto& [id, change] : changes) {
        if (change.begin && (change.begin == change.end)) continue; // Skip if no changes were made
        if (!usages.contains(id)) continue;

        auto& usage = usages.at(id);

        // Skip if the current usage state doesn't match the expected one.
        if (change.begin.has_value() != usage.has_value()) continue;
        if (usage && *change.begin != *usage) continue;

        usage = change.end;
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

template class UsageTracker<renderium::BufferUsages>;
template class UsageTracker<renderium::TextureUsages>;

}