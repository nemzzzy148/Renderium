//
// Created by Nemesis Verstraete on 03/10/2026.
//

#include "TLSF.h"

#include <algorithm>
#include <bit>

namespace rhi {

uint32_t TLSF::createPool(const TLSFPoolDescriptor& descriptor) {
    pools.emplace_back();
    auto& [flCount, slCount, nodes, heads, flBitmap,
        slBitmap, nullNode, unusedNodes] = pools.back();
    const uint64_t size = descriptor.size;
    flCount = std::max<uint32_t>(1u, computeSizeClass(size) + 1u);
    slCount = SL_COUNT;

    // configure starting nodes
    nodes.clear();
    nodes.emplace_back(0, 0, 0, 0, 0, 0, false);
    nodes.emplace_back(0, descriptor.size, 0, 0, 0, 0, true);
    heads.assign(flCount * slCount, 0);
    slBitmap.assign(flCount, 0);

    Pool& p = pools.back();
    p.nodes[0].physicallyNext = 1;
    p.nodes[0].physicallyPrevious = 1;

    p.nodes[1].physicallyPrevious = 0;
    p.nodes[1].physicallyNext = 0;
    listAdd(p, 1);

    return pools.size() - 1;
}

TLSFAllocation TLSF::malloc(const uint32_t pool, const uint32_t alignment, const uint64_t size) {
    if (size == 0 || pool >= pools.size() || alignment == 0 || !std::has_single_bit(alignment)) {
        return {};
    }

    auto& p = pools[pool];

    const uint64_t allocSize = alignUp(size, alignment);
    if (allocSize < size) {
        return {};
    }

    auto [fl, sl] = mapping(allocSize);

    const uint32_t idx = findSuitable(p, fl, sl, allocSize);
    if (idx == 0) {
        return {};
    }

    listRemove(p, idx);

    Node& n = p.nodes[idx];

    if (n.size - allocSize >= MIN_BLOCK) {
        insertPhysicalNode(p, n.offset + allocSize, n.size - allocSize, idx, n.physicallyNext, true);

    }
    n.size = allocSize;
    n.free = false;
    listAdd(p, idx);
    return { n.offset, n.size, idx };

}

TLSFAllocation TLSF::malloc(const uint32_t pool, const uint32_t size) {
    return malloc(pool, defaultAlignment, size);
}

void TLSF::free(const uint32_t pool, const uint32_t node) {
    if (pool >= pools.size()) {
        return;
    }

    Pool& p = pools[pool];

    if (node == 0 || node >= p.nodes.size()) {
        return;
    }

    if (p.nodes[node].free) {
        return;
    }

    p.nodes[node].free = true;
    coalesce(p, node);
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

uint64_t TLSF::alignUp(const uint64_t value, const uint32_t align) {
    if (align == 0 || !std::has_single_bit(align)) {
        return value;
    }
    return (value + static_cast<uint64_t>(align) - 1U) & ~(static_cast<uint64_t>(align) - 1U);
}

void TLSF::listRemove(Pool &pool, const uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size()) {
        return;
    }

    Node& node = pool.nodes[idx];

    if (!node.free) {
        return;
    }

    const auto [fl, sl] = mapping(node.size);
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
    if (idx == 0 || idx >= pool.nodes.size()) {
        return;
    }

    Node& node = pool.nodes[idx];
    const auto [fl, sl] = mapping(node.size);

    if (fl >= pool.flCount || sl >= pool.slCount) {
        return;
    }

    const size_t headIndex =
        static_cast<size_t>(fl) * pool.slCount + sl;

    if (headIndex >= pool.heads.size()) {
        return;
    }

    const uint32_t oldHead = pool.heads[headIndex];

    node.previousFree = 0;
    node.nextFree = oldHead;

    if (oldHead != 0) {
        pool.nodes[oldHead].previousFree = idx;
    }

    pool.heads[headIndex] = idx;

    setBitmaps(pool, fl, sl);
}

uint32_t TLSF::findSuitable(const Pool &pool, const uint32_t fl, const uint32_t sl, const uint64_t requiredSize) {
    if (!pool.flBitmap) return 0;
    for (uint32_t f = fl; f < pool.flCount; ++f) {
        if ((pool.flBitmap & (uint64_t{1} << f)) == 0) {
            continue;
        }
        const uint32_t firstSL = (f == fl) ? sl : 0;

        for (uint32_t s = firstSL; s < pool.slCount; ++s) {
            if ((pool.slBitmap[f] & (uint32_t{1} << s)) == 0) {
                continue;
            }

            const uint32_t headIndex = f * pool.slCount + s;
            for (uint32_t idx = pool.heads[headIndex];
                 idx != 0;
                 idx = pool.nodes[idx].nextFree) {

                if (pool.nodes[idx].size >= requiredSize) {
                    return idx;
                }
            }
        }
    }
    return 0;
}

uint32_t TLSF::takeUnusedNode(Pool &pool) {
    if (pool.unusedNodes.empty()) {
        pool.nodes.emplace_back();
        return pool.nodes.size() - 1;
    }
    const auto idx = pool.unusedNodes.back();
    pool.unusedNodes.pop_back();
    return idx;
}

void TLSF::setBitmaps(Pool &pool, const uint32_t fl, const uint32_t sl) {
    const uint32_t index = fl * pool.slCount + sl;
    const bool nonEmpty = pool.heads[index] != 0;

    if (nonEmpty) {
        pool.slBitmap[fl] |= (uint64_t{1} << sl);
    } else {
        pool.slBitmap[fl] &= ~(uint64_t{1} << sl);
    }

    if (pool.slBitmap[fl] != 0) {
        pool.flBitmap |= (uint64_t{1} << fl);
    } else {
        pool.flBitmap &= ~(uint64_t{1} << fl);
    }
}

uint32_t TLSF::insertPhysicalNode(Pool &pool, const uint64_t offset, const uint64_t size, const uint32_t previous,
    const uint32_t next, const bool isFree) {
    const uint32_t idx = takeUnusedNode(pool);

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
    if (node.physicallyPrevious == 0 &&
        node.physicallyNext == 0 &&
        node.offset == 0 &&
        node.size == 0) {
        return;
    }

    const uint32_t previous = node.physicallyPrevious;
    const uint32_t next = node.physicallyNext;

    pool.nodes[previous].physicallyNext = next;
    pool.nodes[next].physicallyPrevious = previous;

    node.physicallyPrevious = 0;
    node.physicallyNext = 0;
}

uint32_t TLSF::coalesce(Pool &pool, uint32_t idx) {
    if (idx == 0 || idx >= pool.nodes.size()) {
        return 0;
    }

    if (const uint32_t previous = pool.nodes[idx].physicallyPrevious;
            previous != 0 && pool.nodes[previous].free) {
        listRemove(pool, previous);

        pool.nodes[previous].size += pool.nodes[idx].size;

        unlinkPhysicalNode(pool, idx);
        pool.unusedNodes.push_back(idx);

        idx = previous;
    }

    if (const uint32_t next = pool.nodes[idx].physicallyNext;
            next != 0 && pool.nodes[next].free) {
        listRemove(pool, next);

        pool.nodes[idx].size += pool.nodes[next].size;

        unlinkPhysicalNode(pool, next);
        pool.unusedNodes.push_back(next);
    }

    listAdd(pool, idx);

    return idx;
}

}