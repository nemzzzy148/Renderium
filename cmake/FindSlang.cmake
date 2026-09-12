include(FetchContent)

set(SLANG_VERSION "2026.17.1")
set(SLANG_TAG "v${SLANG_VERSION}")

if(APPLE)
    set(os "macos")
elseif(WIN32)
    set(os "windows")
elseif(UNIX)
    set(os "linux")
else()
    message(FATAL_ERROR "Building for unsupported platform with Slang!")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
    set(isa "aarch64")
else()
    set(isa "x86_64")
endif()

set(SLANG_PLATFORM_DIR "${os}-${isa}")

FetchContent_Declare(
        slang_dist
        URL https://github.com/shader-slang/slang/releases/download/${SLANG_TAG}/slang-${SLANG_VERSION}-${SLANG_PLATFORM_DIR}.zip
)
FetchContent_MakeAvailable(slang_dist)

find_package(slang CONFIG REQUIRED
        PATHS "${slang_dist_SOURCE_DIR}/lib/cmake/slang"
        NO_DEFAULT_PATH
)

set(SLANG_BIN_DIR "${slang_dist_SOURCE_DIR}/bin")
set(SLANG_LIB_DIR "${slang_dist_SOURCE_DIR}/lib")
set(SLANG_INCLUDE_DIR "${slang_dist_SOURCE_DIR}/include")

if(NOT EXISTS "${SLANG_LIB_DIR}")
    message(FATAL_ERROR "No Slang libraries found at ${SLANG_LIB_DIR}")
endif()

if(WIN32)
    set(lib "${SLANG_LIB_DIR}/slang.lib")
    set(shared_lib "${SLANG_BIN_DIR}/slang.dll")
elseif(APPLE)
    set(shared_lib "${SLANG_LIB_DIR}/libslang.dylib")
else()
    set(shared_lib "${SLANG_LIB_DIR}/libslang.so")
endif()

if(NOT TARGET slang::slang)
    add_library(slang::slang SHARED IMPORTED GLOBAL)
    set_target_properties(slang::slang PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SLANG_INCLUDE_DIR}")
    if(WIN32)
        set_target_properties(slang::slang PROPERTIES
                IMPORTED_LOCATION "${shared_lib}"
                IMPORTED_IMPLIB "${lib}"
        )
    else()
        set_target_properties(slang::slang PROPERTIES IMPORTED_LOCATION "${shared_lib}")
    endif()
endif()

file(GLOB gl_slang_lib
        "${SLANG_LIB_DIR}/*glslang*.so"
        "${SLANG_LIB_DIR}/*glslang*.dylib"
        "${SLANG_LIB_DIR}/*glslang*.dll"
)

file(GLOB glsl_slang_module_lib
        "${SLANG_LIB_DIR}/*glsl-module*.so"
        "${SLANG_LIB_DIR}/*glsl-module*.dylib"
        "${SLANG_LIB_DIR}/*glsl-module*.dll"
)

file(GLOB _slang_compiler_lib
        "${SLANG_LIB_DIR}/libslang-compiler*"
        "${SLANG_BIN_DIR}/slang-compiler.dll"
)

set(SLANG_RUNTIME_FILES
        ${gl_slang_lib}
        ${glsl_slang_module_lib}
        ${_slang_compiler_lib}
        ${shared_lib}
)

set(SLANG_STANDARD_MODULE_DIR "${SLANG_LIB_DIR}/slang-standard-module-${SLANG_VERSION}")