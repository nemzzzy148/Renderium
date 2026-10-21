//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "BufferImpl.h"

#include "DeviceErrorState.h"

namespace rhi {

bool validateBufferDescriptor(DeviceErrorState &errorState, const renderium::BufferDescriptor &descriptor) {
    if (descriptor.size == 0) {
        errorState.report({
            renderium::ErrorFilter::Validation,
            "The size of a gpu buffer cannot be 0."
        });
        return true;
    }

    return false;
}

}