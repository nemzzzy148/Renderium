//
// Created by Nemesis Verstraete on 18/10/2026.
//

#pragma once

#include <utility>

struct PairHash {
    template<class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& pair) const {
        auto h1 = std::hash<T1>{}(pair.first);
        auto h2 = std::hash<T2>{}(pair.second);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
