//
// Created by Nemesis Verstraete on 03/10/2026.
//

#pragma once
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rhi {

struct TLSFPoolDescriptor {
    uint64_t size;
};

struct TLSFAllocation {
    uint64_t offset = 0;
    uint64_t size = 0;
    uint32_t nodeIndex = 0; // O(1) lookup
};

class TLSF {
public:
    explicit TLSF(const uint32_t defaultAlignment) : defaultAlignment(defaultAlignment) {}

    uint32_t createPool(const TLSFPoolDescriptor& descriptor);

    TLSFAllocation malloc(uint32_t pool, uint32_t alignment, uint64_t size);
    TLSFAllocation malloc(uint32_t pool, uint32_t size);
    void free(uint32_t pool, uint32_t node);

    static constexpr uint16_t MIN_BLOCK = 255;
    static constexpr uint32_t SL_COUNT = 32;
private:
    uint32_t defaultAlignment;
    struct Node {
        uint64_t offset;
        uint64_t size;

        uint32_t physicallyNext;
        uint32_t physicallyPrevious;
        uint32_t nextFree;
        uint32_t previousFree;

        bool free = true;
    };
    struct Pool {
        uint32_t flCount;
        uint32_t slCount;
        std::vector<Node> nodes;
        std::vector<uint32_t> heads;
        uint64_t flBitmap = 0;
        std::vector<uint64_t> slBitmap;
        Node nullNode;
        std::vector<uint32_t> unusedNodes;
    };
    std::vector<Pool> pools;

    static uint32_t computeSubClass(uint64_t size);
    static uint32_t computeSizeClass(uint64_t size);
    static std::pair<uint32_t, uint32_t> mapping(uint64_t size);
    static uint64_t alignUp(uint64_t value, uint32_t align);
    static uint32_t takeUnusedNode(Pool& pool);
    static void setBitmaps(Pool& pool, uint32_t fl, uint32_t sl);
    static void listRemove(Pool& pool, uint32_t idx);
    static void listAdd(Pool& pool, uint32_t idx);
    static uint32_t findSuitable(const Pool& pool, uint32_t fl, uint32_t sl, uint64_t requiredSize);

    static uint32_t insertPhysicalNode(Pool& pool, uint64_t offset, uint64_t size,
        uint32_t previous, uint32_t next, bool isFree);
    static void unlinkPhysicalNode(Pool& pool, uint32_t idx);
    static uint32_t coalesce(Pool& pool, uint32_t idx);
};

}
