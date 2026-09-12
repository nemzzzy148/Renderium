//
// Created by Nemesis Verstraete on 05/09/2026.
//

#pragma once
#include <memory>

#include "Error.h"
#include "Result.h"

namespace renderium {
enum class Backend;
}

namespace shader {

class ShaderCompiler {
public:
    using CompilerResult = renderium::Result<std::unique_ptr<ShaderCompiler>, renderium::Error>;
    static CompilerResult create(renderium::Backend backend);
};

}
