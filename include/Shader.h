//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once

namespace rhi {
template<typename Api>
class ShaderImpl;
}

namespace renderium {

enum class EntryPointType {
    AttributeIndication,
    FunctionName
};

class Shader {
    struct Impl {
        virtual ~Impl() = default;
    };

    explicit Shader(std::unique_ptr<Impl> shader) : shader(std::move(shader)) {}
    std::unique_ptr<Impl> shader;

    template<typename Api>
    friend class rhi::ShaderImpl;
    friend class Device;
};

}