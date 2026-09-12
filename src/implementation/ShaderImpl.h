//
// Created by Nemesis Verstraete on 05/09/2026.
//

#pragma once
#include <algorithm>

#include "Shader.h"

namespace rhi {

template<typename Api>
class ShaderImpl : public renderium::Shader::Impl {
public:
    using Shader = Api::Shader;

private:
    explicit ShaderImpl(Shader shader) : shader(std::move(shader)) {}
    Shader shader;
};

}
