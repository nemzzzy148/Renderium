//
// Created by Nemesis Verstraete on 04/11/2026.
//

#include "ResourceManager.h"
#include "../implementation/IBuffer.h"
#include "../implementation/IPipeline.h"
#include "../implementation/ITexture.h"

namespace resource {
template<typename T>
ResourceManager<T>::ResourceManager(uint32_t reservedSize) {
    resources.reserve(reservedSize);
}

template<typename T>
ResourceId ResourceManager<T>::allocate(std::unique_ptr<T> resource) {
    std::lock_guard lock(registerLock);

    uint32_t idx = 0;
    uint32_t version = 1;
    if (!free.empty()) {
        idx = free.back();
        free.pop_back();
        version = resources[idx].version;
    } else {
        resources.emplace_back();
        idx = resources.size() - 1;
    }

    resources[idx].item = std::move(resource);
    return {idx, version};
}

template<typename T>
void ResourceManager<T>::addRef(uint32_t handle, uint32_t version) {
    std::lock_guard lock(registerLock);
    if (resources.size() <= handle || resources[handle].version != version) return;
    ++resources[handle].refCount;
}

template<typename T>
void ResourceManager<T>::removeRef(uint32_t handle, uint32_t version) {
    std::lock_guard lock(registerLock);
    if (resources.size() <= handle || resources[handle].version != version) return;
    auto refCount = resources[handle].refCount;

    if (refCount == 1) {
        deletionQueue.emplace(handle, version);
    } else {
        --resources[handle].refCount;
    }
}

template<typename T>
const std::unordered_set<ResourceId> & ResourceManager<T>::getDeletionQueue() {
    return deletionQueue;
}

template<typename T>
void ResourceManager<T>::release(uint32_t handle, uint32_t version) {
    std::lock_guard lock(registerLock);
    if (resources.size() <= handle || resources[handle].version != version) return;
    resources[handle].item.reset();
    ++resources[handle].version;
    free.emplace_back(handle);
}

template<typename T>
void ResourceManager<T>::releaseResource(const ResourceId id) {
    if (!deletionQueue.contains(id)) return;
    deletionQueue.erase(id);
    release(id.handle, id.version);
}

template<typename T>
T* ResourceManager<T>::get(uint32_t handle, uint32_t version) {
    std::lock_guard lock(registerLock);
    if (resources.size() <= handle || resources[handle].version != version) return nullptr;
    return resources[handle].item.get();
}

template class ResourceManager<rhi::ITexture>;
template class ResourceManager<rhi::IBuffer>;
template class ResourceManager<rhi::IRenderPipeline>;

}