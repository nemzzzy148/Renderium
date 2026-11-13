//
// Created by Nemesis Verstraete on 13/11/2026.
//

#include "Queue.h"

#include <vector>

namespace renderium {

void Queue::submit(const std::vector<std::reference_wrapper<const CommandBuffer>>& commandBuffers) const {
    impl->submit(commandBuffers);
}

void Queue::submit(const CommandBuffer &commandBuffer) const {
    impl->submit({commandBuffer});
}

}