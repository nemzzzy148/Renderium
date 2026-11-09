//
// Created by Nemesis Verstraete on 09/11/2026.
//

#pragma once
#include <atomic>
#include <cstdint>


namespace synchronization {

class TimelineCounter {
public:
    TimelineCounter() = default;
    uint32_t increase();
    uint32_t get() const;
private:
    std::atomic<uint32_t> value{0};
};

}
