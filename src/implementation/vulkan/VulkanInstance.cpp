//
// Created by Nemesis Verstraete on 15/08/2026.
//

#include "VulkanInstance.h"

#include <vulkan/vulkan.hpp>

#include "../Error.h"

#if defined(_WIN32)
#include <vulkan/vulkan_win32.h>
#elif defined(__APPLE__)
#include <vulkan/vulkan_metal.h>
#elif defined(__ANDROID__)
#include <vulkan/vulkan_android.h>
#elif defined(__linux__)
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>
#include <vulkan/vulkan_wayland.h>
#endif

#include "Instance.h"

namespace rhi::vulkan {

std::vector<char const*> VulkanInstance::validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

VulkanInstance::InstanceResult VulkanInstance::create(const renderium::InstanceCreateInfo &createInfo) {
    vk::raii::Context context;
    vk::ApplicationInfo appInfo{
        .pApplicationName = "ObsidiumGame",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Obsidium",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14
    };
    std::vector<const char*> requiredLayers;
    if (createInfo.debug) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    if (auto availableLayerProperties = context.enumerateInstanceLayerProperties();
        std::ranges::any_of(requiredLayers, [&availableLayerProperties](const auto& layer) {
            return std::ranges::none_of(availableLayerProperties, [layer](const auto& prop) {
                return strcmp(layer, prop.layerName) == 0;
            });
        })) {
        return InstanceResult::err(VulkanError::RequiredLayerMissing);
    }

    auto requiredExtensions = getRequiredSurfaceExtensions();

    if (createInfo.debug) {
        requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
    }
    auto extensionProps = context.enumerateInstanceExtensionProperties();

    const auto it = std::ranges::find_if(requiredExtensions,
        [&extensionProps](const auto& ext) {
        return std::ranges::none_of(extensionProps, [ext](const auto& prop) {
            return strcmp(ext, prop.extensionName) == 0;
        });
    });
    if (it != requiredExtensions.end()) {
        return InstanceResult::err(VulkanError::RequiredExtensionMissing);
    }

    const vk::InstanceCreateInfo instanceInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    auto instance = vk::raii::Instance(context, instanceInfo);

    return InstanceResult::ok(VulkanInstance( std::move(context),
        std::move(instance)));
}

std::vector<char const *> VulkanInstance::getRequiredSurfaceExtensions() {
    std::vector<const char*> extensions;

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_WIN32)
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
    extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
    extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
    extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

    return extensions;
}

}