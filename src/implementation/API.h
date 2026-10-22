//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include "Shader.h"
#include "Window.h"
#include "vulkan/vulkan.h"
#include "vulkan/VulkanConversion.h"

namespace rhi {

// --- Graphics API ---

// Vulkan
namespace vulkan {
class VulkanQueue;
class VulkanSurface;
class VulkanAdapter;
class VulkanDevice;
class VulkanInstance;
enum class VulkanError;
class VulkanRenderPipeline;
class VulkanComputePipeline;
class VulkanBuffer;
class VulkanTexture;
class VulkanTextureView;
}

struct VulkanApi {
    using Instance = vulkan::VulkanInstance;
    using Adapter = vulkan::VulkanAdapter;
    using Device = vulkan::VulkanDevice;
    using Queue = vulkan::VulkanQueue;
    using InternalSurface = VkSurfaceKHR;
    using Surface = vulkan::VulkanSurface;
    using Error = vulkan::VulkanError;
    using RenderPipeline = vulkan::VulkanRenderPipeline;
    using ComputePipeline = vulkan::VulkanComputePipeline;
    using Buffer = vulkan::VulkanBuffer;
    using Texture = vulkan::VulkanTexture;
    using TextureView = vulkan::VulkanTextureView;
    static constexpr renderium::shader::ShadingOutputLanguage shaderOutputLanguage =
        renderium::shader::ShadingOutputLanguage::SPIRV;

    static constexpr auto toError = &vulkan::VulkanConversion::toError;
    static constexpr auto toRequestError = &vulkan::VulkanConversion::toRequestError;
};

// --- Window API ---

namespace window {

// GLFW
namespace glfw {
enum class GlfwError;
class GlfwWindow;
}

struct GlfwApi {
    using Window = glfw::GlfwWindow;
    using Error = glfw::GlfwError;
};

template<renderium::WindowBackend backend>
struct windowBackendStruct;

template<> struct windowBackendStruct<renderium::WindowBackend::Glfw> { using type = GlfwApi; };

template<renderium::WindowBackend backend>
using WindowBackendType = windowBackendStruct<backend>::type;

}

// --- Shader API ---

namespace shader {

namespace slang {
class SlangCompiler;
}

struct SlangApi {
    using ShaderCompiler = slang::SlangCompiler;
};

template<renderium::ShadingLanguage>
struct ShaderCompilerTypeWrapper;

template<> struct ShaderCompilerTypeWrapper<renderium::ShadingLanguage::SLANG> { using Type = SlangApi; };

template<renderium::ShadingLanguage ShadingLanguage>

using ShaderCompilerType = ShaderCompilerTypeWrapper<ShadingLanguage>::Type;

}

}
