//
// Created by Nemesis Verstraete on 27/07/2026.
//

#pragma once

namespace rhi {
template<typename Api>
class ShaderImpl;
template<typename Api>
class ShaderCompilerImpl;
}

namespace renderium {

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

class ShaderCompiler {
public:
    struct Impl {
        virtual ~Impl() = default;

        virtual Result<std::string, Error> compileShader(const std::string& shaderCode) = 0;
    };
    explicit ShaderCompiler(std::unique_ptr<Impl> shaderCompiler) : shaderCompiler(std::move(shaderCompiler)) {}

    using ShaderResult = Result<std::string, Error>;
    ShaderResult compileShader(const std::string& shaderCode);
private:
    std::unique_ptr<Impl> shaderCompiler;

    template<typename Api>
    friend class rhi::ShaderCompilerImpl;
};

}