//
// Created by Nemesis Verstraete on 15/08/2026.
//

#pragma once
#include "Window.h"
#include "ShaderCompiler.h"
#include "vulkan/vulkan.h"

namespace rhi {

// --- Graphics API ---

// Vulkan
namespace vulkan {
class VulkanQueue;
class VulkanSurface;
class VulkanDevice;
class VulkanDeviceSelector;
class VulkanInstance;
enum class VulkanError;
class VulkanRenderPipeline;
class VulkanComputePipeline;
}

struct VulkanApi {
    using Instance = vulkan::VulkanInstance;
    using DeviceSelector = vulkan::VulkanDeviceSelector;
    using Device = vulkan::VulkanDevice;
    using Queue = vulkan::VulkanQueue;
    using InternalSurface = VkSurfaceKHR;
    using Surface = vulkan::VulkanSurface;
    using Error = vulkan::VulkanError;
    using RenderPipeline = vulkan::VulkanRenderPipeline;
    using ComputePipeline = vulkan::VulkanComputePipeline;
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

// slang

namespace shader {

namespace slang {
class SlangCompiler;
}

struct SlangApi {
    using ShaderCompiler = slang::SlangCompiler;
};

template<renderium::shader::ShadingLanguage>
struct ShaderCompilerTypeWrapper;

template<> struct ShaderCompilerTypeWrapper<renderium::shader::ShadingLanguage::SLANG> { using Type = SlangApi; };

template<renderium::shader::ShadingLanguage ShadingLanguage>

using ShaderCompilerType = ShaderCompilerTypeWrapper<ShadingLanguage>::Type;

}

}