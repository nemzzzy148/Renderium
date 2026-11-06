//
// Created by Nemesis Verstraete on 06/11/2026.
//

#pragma once
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../resource/ResourceManager.h"

namespace renderium {
enum class BufferUsages : uint16_t;
enum class TextureUsages : uint8_t;
}

namespace track {

template<typename T>
concept Usage = std::is_same_v<renderium::TextureUsages, std::remove_cvref<T>> ||
                std::is_same_v<renderium::BufferUsages, std::remove_cvref<T>>;

template<Usage T>
struct Merge {
    T begin;
    T end;
};

template<Usage T>
class ScopeTracker {
public:
    explicit ScopeTracker(const std::vector<std::pair<resource::ResourceId, T>>& beginUsages);
    void setUsage(resource::ResourceId id, T usage);

    const std::unordered_map<resource::ResourceId, Merge<T>>& getChanges() const { return changes; }
private:
    std::unordered_map<resource::ResourceId, Merge<T>> changes;
};

}
