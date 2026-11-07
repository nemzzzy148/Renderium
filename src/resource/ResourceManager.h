//
// Created by Nemesis Verstraete on 04/11/2026.
//

#pragma once
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace resource {

struct ResourceId {
    uint32_t handle;
    uint32_t version;

    bool operator==(const ResourceId&) const = default;
};

template<typename T>
class ResourceManager {
public:
    explicit ResourceManager(uint32_t reservedSize = 1024);

    ResourceId allocate(std::unique_ptr<T> resource);

    void addRef(uint32_t handle, uint32_t version);
    void removeRef(uint32_t handle, uint32_t version);

    T* get(uint32_t handle, uint32_t version);
private:
    struct Resource {
        std::unique_ptr<T> item;
        uint32_t version = 1;
        uint32_t refCount = 1;
    };
    std::vector<Resource> resources;
    std::vector<uint32_t> free;

    std::queue<uint32_t> pendingDeletion;

    void release(uint32_t handle, uint32_t version);

    std::mutex registerLock;
};

}

template<>
struct std::hash<resource::ResourceId> {
    std::size_t operator()(const resource::ResourceId& id) const noexcept {
        return hash<uint32_t>{}(id.handle) ^ (hash<uint32_t>{}(id.version) << 1);
    }
};
