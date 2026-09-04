//
// Created by Nemesis Verstraete on 17/08/2026.
//

#pragma once

namespace rhi {

enum class Error {
    UnhandledException,
    SurfaceCreationFailed
};


namespace vulkan {

/*
 * If there is a bug in renderium while trying to catch an error, it could return an unhandled error.
 * The integer linked with this unhandled error is always 0.
*/
enum class VulkanError {
    UnhandledException,
    RequiredExtensionMissing,
    RequiredLayerMissing,
    WindowBackendNotRecognized,
    WindowBackendFailedToCreateSurface,
    NoPhysicalDevicesFound,
    NoPhysicalDevicesSuitable,
    NoQueueFamilySuitable
};

}
namespace window::glfw {

enum class GlfwError {
    UnhandledException,
    CreateInfoOtherBackend,
    FailedToInit,
    FailedToCreateWindow,
    FailedToCreateVulkanSurface
};

}
}