//
// Created by Nemesis Verstraete on 21/10/2026.
//

#include "BufferImpl.h"

#include "DeviceErrorState.h"

namespace rhi {

void validateBufferDescriptor(Process& process, const renderium::BufferDescriptor &descriptor) {
    if (descriptor.size == 0) {
        process.report({
            renderium::ErrorFilter::Validation,
            "Buffer creation requires the size to be greater than 0."
        });
    }
}

}