//
// Created by Nemesis Verstraete on 05/11/2026.
//

#include <Buffer.h>

#include "utils/BitwiseOperations.h"

namespace renderium {

Buffer::Buffer(std::shared_ptr<detail::Resource> resource, const BufferDescriptor &descriptor)
    : size(descriptor.size), usage(descriptor.usage), resource(std::move(resource)) {}

}