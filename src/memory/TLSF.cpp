//
// Created by Nemesis Verstraete on 03/10/2026.
//

#include "TLSF.h"

#include <bit>

namespace rhi {

TLSF::TLSF(const uint32_t defaultAlignment) : defaultAlignment(defaultAlignment) {
    // index 0 is reserved for an invalid pool
    pools.emplace_back();
    poolHeads.assign(64, 0);
}

TLSF::TLSF() : defaultAlignment(0) {
    TLSF(0);
}

uint32_t TLSF::createPool(const TLSFPoolDescriptor& descriptor) {
    if (descriptor.size == 0) {
        return 0;
    }

    // FL-bitmap is 64 bits; reject sizes outside the supported range.
    const uint32_t fl = computeSizeClass(descriptor.size);
    if (fl >= 64) {
        return 0;
    }

    Pool pool{};
    pool.slCount = SL_COUNT;
    pool.flCount = fl + 1;
    pool.nodes.reserve(2);

    pool.nodes.emplace_back();
    pool.nodes[0].free = false;
    pool.nodes.emplace_back();
    pool.nodes[1].offset = 0;
    pool.nodes[1].size = descriptor.size;
    pool.nodes[1].free = true;

    pool.heads.assign(static_cast<size_t>(pool.flCount) * pool.slCount, 0);
    pool.slBitmap.assign(pool.flCount, 0);

    // The two nodes form a circular physical list.
    pool.nodes[0].physicallyNext = 1;
    pool.nodes[0].physicallyPrevious = 1;
    pool.nodes[1].physicallyPrevious = 0;
    pool.nodes[1].physicallyNext = 0;

    pools.emplace_back(std::move(pool));

    const auto poolIndex =
        static_cast<uint32_t>(pools.size() - 1);
    listAdd(pools[poolIndex], 1);

    poolListAdd(poolIndex);

    return poolIndex;
}

TLSFAllocation TLSF::allocate(const uint32_t pool, const uint32_t alignment, const uint64_t size) {
    if (size == 0 || pool >= pools.size() || alignment == 0 || !std::has_single_bit(alignment)) {
        return {};
    }

    uint64_t allocSize;
    if (!alignUp(size, defaultAlignment, allocSize)) return {};
    allocSize += defaultAlignment - 1;                   // worst-case

    auto& p = pools[pool];
    auto [fl, sl] = mapping(allocSize);

    const uint32_t idx = findSuitable(p, fl, sl, allocSize, alignment);
    if (idx == 0) {
        return {};
    }

    Node& n = p.nodes[idx];

    // original node values
    const uint64_t originalOffset = n.offset;
    const uint64_t originalSize = n.size;

    const uint32_t flBefore = bitmapSizeClass(p.flBitmap);
    listRemove(p, idx);

    const uint64_t aligned = (originalOffset + alignment - 1) & ~(static_cast<uint64_t>(alignment) - 1);
    const uint64_t used = (aligned - originalOffset) + allocSize;

    n.offset = aligned;
    n.size = allocSize;
    n.free = false;
    n.nextFree = n.previousFree = 0;

    if (originalSize - allocSize >= MIN_BLOCK) {
        insertPhysicalNode(p, n.offset + used, originalSize - allocSize, idx, n.physicallyNext, true);
        p.nodes[idx].size = used;
    }

    // if pool bitmaps not the same, update the free pools
    if (bitmapSizeClass(p.flBitmap) != flBefore) updateFreePoolList(pool);

    return { aligned, allocSize, idx, pool, true };
}

TLSFAllocation TLSF::allocate(const uint64_t size) {
    if (size == 0) return {};

    return allocate(defaultAlignment, size);
}

TLSFAllocation TLSF::allocate(const uint32_t alignment, const uint64_t size) {
    if (size == 0) return {};

    const uint32_t sizeClass = computeSizeClass(alignment - 1);

    for (uint32_t i = poolHeads[sizeClass]; i != 0; i = pools[i].nextFree)
        if (auto a = allocate(i, alignment, size); a.valid) return a; // try pools

    const uint64_t mask = freePools & (~0ull << (sizeClass + 1));
    if (mask == 0) return {};
    const uint32_t i = poolHeads[std::countr_zero(mask)];

    return allocate(i, alignment, size);
}

void TLSF::free(const uint32_t pool, const uint32_t node) {
    if (pool == 0 || pool >= pools.size()) {
        return;
    }

    Pool& p = pools[pool];
    if (node == 0 || node >= p.nodes.size()) {
        return;
    }

    Node& n = p.nodes[node];
    if (n.free || n.size == 0) {
        return;
    }

    n.free = true;

    const auto originalBitmap = p.flBitmap;
    coalesce(p, node);

    // update free lists if a new bigger block joined
    if (std::bit_width(originalBitmap) < std::bit_width(p.flBitmap)) {
        updateFreePoolList(pool);
    }
}

void TLSF::poolListAdd(const uint32_t idx) {
    if (idx >= pools.size() || idx == 0) return;

    Pool& p = pools[idx];
    if (p.flBitmap == 0 || p.listedClass != NOT_LISTED) return;
    const auto sizeClass = bitmapSizeClass(p.flBitmap);
    uint32_t& head = poolHeads[sizeClass];

    p.listedClass = sizeClass;
    p.nextFree = head;
    p.previousFree = 0;

    if (head) pools[head].previousFree = idx;
    head = idx;

    freePools |= 1ull << sizeClass;
}

void TLSF::poolListRemove(const uint32_t idx) {
    if (idx >= pools.size() || idx == 0) return;

    Pool& p = pools[idx];
    if (p.listedClass == NOT_LISTED) return;
    const uint32_t sizeClass = p.listedClass;

    // check if next and previous element isn't the null pool
    if (p.previousFree) pools[p.previousFree].nextFree = p.nextFree;
    else poolHeads[sizeClass] = p.nextFree;
    if (p.nextFree) pools[p.nextFree].previousFree = p.previousFree;

    // change bitmaps when size class is empty
    if (poolHeads[sizeClass] == 0) freePools &= ~(1ull << sizeClass);

    p.nextFree = p.previousFree = 0;
    p.listedClass = NOT_LISTED;
}

void TLSF::setPoolFree(const uint32_t sizeClass, const bool value) {
    if (sizeClass == 0) return;

    if (value) {
        freePools |= 1ull << sizeClass;
    } else {
        freePools &= ~(1ull << sizeClass);
    }
}

void TLSF::updateFreePoolList(const uint32_t idx) {
    poolListRemove(idx);
    poolListAdd(idx);
}

uint32_t TLSF::bitmapSizeClass(const uint64_t bitmap) {
    if (bitmap == 0) return -1; // no free blocks :=(
    return std::bit_width(bitmap) - 1;
}

uint32_t TLSF::computeSubClass(const uint64_t size) {
    if (size == 0) return  0;
    if (size < 256) {
        return static_cast<uint32_t>((size - 1) >> 3);
    }
    const uint32_t bits = std::bit_width(size);
    const uint32_t shift = bits - 6;

    return static_cast<uint32_t>(
        (size >> shift) - 32
    );
}

uint32_t TLSF::computeSizeClass(const uint64_t size) {
    if (size < 256) {
        return 0;
    }
    return static_cast<uint32_t>(std::bit_width(size) - 8);
}

std::pair<uint32_t, uint32_t> TLSF::mapping(const uint64_t size) {
    if (size == 0 || size < 256) {
        return {0, computeSubClass(size)};
    }

    return {
        computeSizeClass(size),
        computeSubClass(size)
    };
}

bool TLSF::alignUp(const uint64_t value, const uint32_t alignment, uint64_t& result) {
    if (alignment == 0 || !std::has_single_bit(alignment)) {
        return false;
    }
    const uint64_t mask = static_cast<uint64_t>(alignment) - 1ULL;
    // detect overflow
    if (value > std::numeric_limits<uint64_t>::max() - mask) {
        return false;
    }
    result = (value + mask) & ~mask;
    return true;
}

uint32_t TLSF::takeUnusedNode(Pool &pool) {
    if (!pool.unusedNodes.empty()) {
        const uint32_t idx = pool.unusedNodes.back();
        pool.unusedNodes.pop_back();
        pool.nodes[idx] = Node{};
        return idx;
    }

    if (pool.nodes.size() >= std::numeric_limits<uint32_t>::max()) {
        return 0;
    }

    pool.nodes.emplace_back();
    return static_cast<uint32_t>(pool.nodes.size() - 1);
}

void TLSF::setBitmaps(Pool &pool, const uint32_t fl, const uint32_t sl) {
    if (fl >= pool.flCount || sl >= pool.slCount) {
        return;
    }
    const uint32_t index = fl * pool.slCount + sl;
    const bool nonEmpty = pool.heads[index] != 0;

    const uint64_t slBit = uint64_t{1} << sl;
    const uint64_t flBit = uint64_t{1} << fl;

    if (nonEmpty) {
        pool.slBitmap[fl] |= slBit;
    } else {
        pool.slBitmap[fl] &= ~slBit;
    }

    if (pool.slBitmap[fl] != 0) {
        pool.flBitmap |= flBit;
    } else {
        pool.flBitmap &= ~flBit;
    }
}

void TLSF::listRemove(Pool &pool, const uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size() || !pool.nodes[idx].free) {
        return;
    }

    Node& node = pool.nodes[idx];
    const auto [fl, sl] = mapping(node.size);
    if (fl >= pool.flCount || sl >= pool.slCount) {
        return;
    }
    const uint32_t headIndex = fl * pool.slCount + sl;

    if (node.previousFree != 0) {
        pool.nodes[node.previousFree].nextFree = node.nextFree;
    } else {
        pool.heads[headIndex] = node.nextFree;
    }
    if (node.nextFree != 0) {
        pool.nodes[node.nextFree].previousFree = node.previousFree;
    }

    node.previousFree = 0;
    node.nextFree = 0;
    setBitmaps(pool, fl, sl);
}

void TLSF::listAdd(Pool &pool, const uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size() || !pool.nodes[idx].free || pool.nodes[idx].size == 0) {
        return;
    }

    Node& node = pool.nodes[idx];
    const auto [fl, sl] = mapping(node.size);
    if (fl >= pool.flCount || sl >= pool.slCount) {
        return;
    }

    const uint32_t headIndex = fl * pool.slCount + sl;
    const uint32_t oldHead = pool.heads[headIndex];
    node.previousFree = 0;
    node.nextFree = oldHead;

    if (oldHead != 0) {
        pool.nodes[oldHead].previousFree = idx;
    }
    pool.heads[headIndex] = idx;
    setBitmaps(pool, fl, sl);
}

uint32_t TLSF::findSuitable(const Pool &pool, const uint32_t fl, const uint32_t sl, const uint64_t requiredSize, uint32_t alignment) {
    if (fl >= pool.flCount || sl >= pool.slCount || pool.flBitmap == 0) return 0;

    for (uint32_t f = fl; f < pool.flCount; ++f) {
        if ((pool.flBitmap & (uint64_t{1} << f)) == 0) {
            continue;
        }
        const uint32_t firstSL = (f == fl) ? sl : 0;
        for (uint32_t s = firstSL; s < pool.slCount; ++s) {
            if ((pool.slBitmap[f] & (uint64_t{1} << s)) == 0) {
                continue;
            }
            const uint32_t headIndex = f * pool.slCount + s;
            for (uint32_t idx = pool.heads[headIndex]; idx != 0;
                 idx = pool.nodes[idx].nextFree) {
                const Node& node = pool.nodes[idx];
                uint64_t alignedOffset = 0;
                if (!alignUp(node.offset, alignment, alignedOffset)) {
                    continue;
                }

                // ensure proper fit
                const uint64_t prefix = alignedOffset - node.offset;
                if (prefix <= node.size && requiredSize <= node.size - prefix) {
                    return idx;
                }
            }
        }
    }
    return 0;
}

uint32_t TLSF::insertPhysicalNode(Pool &pool, const uint64_t offset, const uint64_t size, const uint32_t previous,
    const uint32_t next, const bool isFree) {
    const uint32_t idx = takeUnusedNode(pool);
    if (idx == 0 || previous >= pool.nodes.size() || next >= pool.nodes.size()) {
        return 0;
    }

    pool.nodes[idx].offset = offset;
    pool.nodes[idx].size = size;
    pool.nodes[idx].free = isFree;
    pool.nodes[idx].physicallyPrevious = previous;
    pool.nodes[idx].physicallyNext = next;
    pool.nodes[idx].previousFree = 0;
    pool.nodes[idx].nextFree = 0;

    pool.nodes[previous].physicallyNext = idx;
    pool.nodes[next].physicallyPrevious = idx;

    if (isFree) {
        listAdd(pool, idx);
    }
    return idx;
}

void TLSF::unlinkPhysicalNode(Pool &pool, uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size()) {
        return;
    }

    Node& node = pool.nodes[idx];
    const uint32_t previous = node.physicallyPrevious;
    const uint32_t next = node.physicallyNext;
    if (previous >= pool.nodes.size() || next >= pool.nodes.size()) {
        return;
    }

    pool.nodes[previous].physicallyNext = next;
    pool.nodes[next].physicallyPrevious = previous;
    node.physicallyPrevious = 0;
    node.physicallyNext = 0;
    node.offset = 0;
    node.size = 0;
    node.free = false;
    node.nextFree = 0;
    node.previousFree = 0;
}

uint32_t TLSF::coalesce(Pool &pool, uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size() || !pool.nodes[idx].free) {
        return 0;
    }

    // remove neighbouring free nodes from their bins
    uint32_t previous = pool.nodes[idx].physicallyPrevious;
    if (previous != 0 && pool.nodes[previous].free) {
        listRemove(pool, previous);
        pool.nodes[previous].size += pool.nodes[idx].size;
        unlinkPhysicalNode(pool, idx);
        pool.unusedNodes.push_back(idx);
        idx = previous;
    }

    const uint32_t next = pool.nodes[idx].physicallyNext;
    if (next != 0 && pool.nodes[next].free) {
        listRemove(pool, next);
        pool.nodes[idx].size += pool.nodes[next].size;
        unlinkPhysicalNode(pool, next);
        pool.unusedNodes.push_back(next);
    }

    listAdd(pool, idx);
    return idx;
}

}