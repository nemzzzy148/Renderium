//
// Created by Nemesis Verstraete on 06/11/2026.
//

#include "ScopeTracker.h"
#include <ranges>

namespace track {
template<Usage T>
void ScopeTracker<T>::addUsage(resource::ResourceId id, std::optional<T> beginUsage, T endUsage) {
    changes.insert_or_assign(id, Merge<T>{beginUsage, endUsage});
}

template<Usage T>
void ScopeTracker<T>::addBeginUsage(resource::ResourceId id, std::optional<T> usage) {
    changes[id].begin = usage;
}

template<Usage T>
void ScopeTracker<T>::setEndUsage(resource::ResourceId id, T usage) {
    if (!changes.contains(id)) return;
    changes.at(id).end = usage;
}

template<Usage T>
bool ScopeTracker<T>::hasUsage(resource::ResourceId id) {
    return changes.contains(id);
}

}