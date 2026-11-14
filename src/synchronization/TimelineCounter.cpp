//
// Created by Nemesis Verstraete on 09/11/2026.
//

#include "TimelineCounter.h"

namespace synchronization {

uint32_t TimelineCounter::increase() {
    return value.fetch_add(1, std::memory_order_release);
}

uint32_t TimelineCounter::get() const {
    return value.load(std::memory_order_acquire);
}

void TimelineCounter::set(const uint32_t index) {
    if (index <= get()) return;
    value.store(index, std::memory_order_release);
}

}