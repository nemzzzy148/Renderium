//
// Created by Nemesis Verstraete on 06/11/2026.
//

#pragma once
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <Buffer.h>

#include "../resource/ResourceManager.h"

namespace renderium {
enum class BufferUsages : uint16_t;
enum class TextureUsages : uint8_t;
}

namespace track {

template<typename T>
concept Usage = std::is_same_v<renderium::TextureUsages, std::remove_cvref_t<T>> ||
                std::is_same_v<renderium::BufferUsages, std::remove_cvref_t<T>>;

template<Usage T>
struct Merge {
    std::optional<T> begin; // The resource may be uninitialized at creation.
    T end;
};

template<Usage T>
using BeginUsages = std::vector<std::pair<resource::ResourceId, T>>;

template<Usage T>
class ScopeTracker {
public:
    ScopeTracker() = default;
    void setUsage(resource::ResourceId id, T usage);

    void initialize(const BeginUsages<T>& beginUsages);
    const std::unordered_map<resource::ResourceId, Merge<T>>& getChanges() const { return changes; }

    void reset();
private:
    std::unordered_map<resource::ResourceId, Merge<T>> changes;
};

}