//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once

namespace renderium {

enum class ErrorFilter{
    Validation,
    OutOfMemory,
    Internal
};

struct Error {
    ErrorFilter type;
    std::string message;
};

enum class RequestError {
    UnhandledException,
    InstanceCreateError,
    RequestAdapterError,
    RequestDeviceError,
    QueueCreateError,
    SurfaceCreateError,
    ShaderCompilationError,
    FailedToCreateShaderCompiler,
    UnknownShadingLanguage,
    RenderPipelineCreateError
};

}