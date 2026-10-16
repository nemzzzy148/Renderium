//
// Created by Nemesis Verstraete on 03/10/2026.
//

#pragma once
#include <cstdint>
#include <utility>
#include <vector>

namespace rhi {

struct TLSFPoolDescriptor {
    uint64_t size = 0;
};

struct TLSFAllocation {
    uint64_t offset = 0;
    uint64_t size = 0;
    uint32_t nodeIndex = 0; // O(1) lookup
    uint32_t poolIndex = 0;
    bool valid = false;
};

class TLSF {
public:
    explicit TLSF(uint32_t defaultAlignment);

    uint32_t createPool(const TLSFPoolDescriptor& descriptor);

    TLSFAllocation malloc(uint32_t pool, uint32_t alignment, uint64_t size);
    TLSFAllocation malloc(uint32_t pool, uint64_t size);
    TLSFAllocation malloc(uint64_t size);
    void free(uint32_t pool, uint32_t node);

    static constexpr uint16_t MIN_BLOCK = 255;
    static constexpr uint32_t SL_COUNT = 32;
    static constexpr uint32_t NOT_LISTED = UINT32_MAX;
private:
    uint32_t defaultAlignment;

    struct Node {
        uint64_t offset = 0;
        uint64_t size = 0;
        uint32_t physicallyNext = 0;
        uint32_t physicallyPrevious = 0;
        uint32_t nextFree = 0;
        uint32_t previousFree = 0;
        bool free = true;
    };

    struct Pool {
        uint32_t flCount = 0;
        uint32_t slCount = 0;
        std::vector<Node> nodes;
        std::vector<uint32_t> heads;
        uint64_t flBitmap = 0;
        std::vector<uint64_t> slBitmap;
        std::vector<uint32_t> unusedNodes;

        uint32_t nextFree;
        uint32_t previousFree;
        uint32_t listedClass;
    };

    // bitmaps and variables for fast allocation between pools
    std::vector<Pool> pools;
    uint64_t freePools = 0;
    std::vector<uint32_t> poolHeads;

    // size classes map to 256 bytes, so size class 0 -> 256, 1 -> 512, just like fl

    void poolListAdd(uint32_t idx);
    void poolListRemove(uint32_t idx);
    void setPoolFree(uint32_t sizeClass, bool value);
    void updateFreePoolList(uint32_t idx);
    uint32_t findSuitablePool(uint64_t size);

    static uint32_t bitmapSizeClass(uint64_t bitmap);
    static uint32_t ceilSizeClass(uint64_t size, uint32_t alignment);

    static uint32_t computeSubClass(uint64_t size);
    static uint32_t computeSizeClass(uint64_t size);
    static std::pair<uint32_t, uint32_t> mapping(uint64_t size);
    static bool alignUp(uint64_t value, uint32_t alignment, uint64_t& result);
    static uint32_t takeUnusedNode(Pool& pool);
    static void setBitmaps(Pool& pool, uint32_t fl, uint32_t sl);
    static void listRemove(Pool& pool, uint32_t idx);
    static void listAdd(Pool& pool, uint32_t idx);
    static uint32_t findSuitable(const Pool& pool, uint32_t fl, uint32_t sl,
        uint64_t requiredSize, uint32_t alignment);

    static uint32_t insertPhysicalNode(Pool& pool, uint64_t offset, uint64_t size,
        uint32_t previous, uint32_t next, bool isFree);
    static void unlinkPhysicalNode(Pool& pool, uint32_t idx);
    static uint32_t coalesce(Pool& pool, uint32_t idx);
};

}
