cmake_minimum_required(VERSION 4.2)
project(Renderium)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Vulkan REQUIRED)

find_package(glfw3 REQUIRED)

include_directories(/opt/homebrew/include)

add_library(Renderium SHARED
        src/Instance.cpp
        src/Surface.cpp

        src/implementation/vulkan/VulkanInstance.cpp
        src/implementation/vulkan/VulkanSurface.cpp
        src/implementation/vulkan/VulkanPipeline.cpp
        src/implementation/vulkan/VulkanDevice.cpp
        sandbox/main.cpp
        include/Window.h
        src/implementation/WindowImpl.h
        src/implementation/window/glfw/GlfwWindow.cpp
        src/implementation/window/glfw/GlfwWindow.h
        src/Window.cpp
        src/implementation/Error.h
        src/implementation/DeviceImpl.h
        src/utils/BitwiseOperations.h
        src/Device.cpp
        src/implementation/vulkan/VulkanQueue.cpp
        src/implementation/vulkan/VulkanQueue.h
        src/implementation/QueueImpl.h
        include/Renderium.h
        include/ComputeRenderium.h
)

target_include_directories( Renderium PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

enable_testing()

add_subdirectory(sandbox)

target_compile_definitions(Renderium PUBLIC VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

target_link_libraries( Renderium
        Vulkan::Vulkan
        glfw
        slang
)