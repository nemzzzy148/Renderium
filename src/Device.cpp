//
// Created by Nemesis Verstraete on 30/08/2026.
//

#include "Device.h"

#include "Queue.h"

namespace renderium {

Queue Device::getQueue() const {
    if (!queueImpl) // normally this error is impossible
        throw std::runtime_error("Renderium fatal error: failed to get queue on device!");
    return Queue(*queueImpl);
}

}
