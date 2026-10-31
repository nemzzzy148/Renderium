//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once

namespace rhi {

// --- Graphics API's Errors ---

// Vulkan
namespace vulkan {
enum class VulkanError {
    UnhandledException,
    FailedToCreateVulkanInstance,
    UnableToListLayers,
    UnableToListExtensions,
    RequiredExtensionMissing,
    RequiredLayerMissing,
    WindowBackendFailedToCreateSurface,
    SwapChainCreationFailed,
    SwapChainOutOfDate,
    UnableToGetSwapChainImages,
    FailedToCreateSwapChainSemaphores,
    FailedToCreateSwapChainImageViews,
    NoPhysicalDevicesFound,
    NoPhysicalDevicesSuitable,
    NoQueueFamilySuitable,
    NoCompatibleMemoryTypeFound,
    OutOfDeviceMemory,
    OutOfHostMemory,
    InvalidBufferSize,
    InvalidImageDimensions,
    VertexEntryPointNotFound,
    FragmentEntryPointNotFound,
    ShaderModuleCreationError,
    PipelineLayoutCreationError,
    PipelineCreationError,
};
}

// --- Window API's Errors ---

// GLFW
namespace window::glfw {
enum class GlfwError {
    UnhandledException,
    WindowDescriptorBackendMismatch,
    FailedToInit,
    FailedToCreateWindow,
    FailedToCreateVulkanSurface
};
}

// --- Shader API's Errors ---

// Slang
namespace shader::slang {
enum class SlangError {
    UnsupportedShadingLanguage,
    UnknownTargetProfile,
    FailedToCreateGlobalSession,
    FailedToCreateSession,
    FailedToCreateSlangShaderModule,
    ShaderCompilationError,
    MissingShaderEntryPoints,
    FailedToComposeShader,
    FailedToLinkShader,
    FailedToGenerateEntryPointCode,
};
}

}