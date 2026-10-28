#pragma once

#include <cstdint>
#include <string>

namespace renderium {

enum class SupportedFeatures : uint32_t {
    AtomicVec2uMinMax = 1,
    CoreFeaturesAndLimits = 1 << 1,
    DepthClipControl = 1 << 2,
    Depth32FloatStencil8 = 1 << 3,
    TextureCompressionBC = 1 << 4,
    TextureCompressionBCSliced3D = 1 << 5,
    TextureCompressionETC2 = 1 << 6,
    TextureCompressionASTC = 1 << 7,
    TextureCompressionASTCSliced3D = 1 << 8,
    TimestampQuery = 1 << 9,
    IndirectFirstInstance = 1 << 10,
    ShaderF16 = 1 << 11,
    RG11B10UfloatRenderable = 1 << 12,
    BGRA8UnormStorage = 1 << 13,
    Float32Filterable = 1 << 14,
    Float32Blendable = 1 << 15,
    ClipDistances = 1 << 16,
    DualSourceBlending = 1 << 17,
    Subgroups = 1 << 18,
    TextureFormatsTier1 = 1 << 19,
    TextureFormatsTier2 = 1 << 20,
    PrimitiveIndex = 1 << 21,
    TextureComponentSwizzle = 1 << 22,
    SubgroupSizeControl = 1 << 23,
    TextureCompressionUnaligned = 1 << 24
};

struct SupportedLimits {
    uint32_t maxTextureDimension1D;
    uint32_t maxTextureDimension2D;
    uint32_t maxTextureDimension3D;
    uint32_t maxTextureArrayLayers;
    uint32_t maxBindGroups;
    uint32_t maxBindGroupsPlusVertexBuffers;
    uint32_t maxImmediateSize;
    uint32_t maxBindingsPerBindGroup;
    uint32_t maxDynamicUniformBuffersPerPipelineLayout;
    uint32_t maxDynamicStorageBuffersPerPipelineLayout;
    uint32_t maxSampledTexturesPerShaderStage;
    uint32_t maxSamplersPerShaderStage;
    uint32_t maxStorageBuffersPerShaderStage;
    uint32_t maxStorageBuffersInVertexStage;
    uint32_t maxStorageBuffersInFragmentStage;
    uint32_t maxStorageTexturesPerShaderStage;
    uint32_t maxStorageTexturesInVertexStage;
    uint32_t maxStorageTexturesInFragmentStage;
    uint32_t maxUniformBuffersPerShaderStage;
    uint64_t maxUniformBufferBindingSize;
    uint64_t maxStorageBufferBindingSize;
    uint32_t minUniformBufferOffsetAlignment;
    uint32_t minStorageBufferOffsetAlignment;
    uint32_t maxVertexBuffers;
    uint64_t maxBufferSize;
    uint32_t maxVertexAttributes;
    uint32_t maxVertexBufferArrayStride;
    uint32_t maxInterStageShaderVariables;
    uint32_t maxColorAttachments;
    uint32_t maxColorAttachmentBytesPerSample;
    uint32_t maxComputeWorkgroupStorageSize;
    uint32_t maxComputeInvocationsPerWorkgroup;
    uint32_t maxComputeWorkgroupSizeX;
    uint32_t maxComputeWorkgroupSizeY;
    uint32_t maxComputeWorkgroupSizeZ;
    uint32_t maxComputeWorkgroupsPerDimension;
};

struct AdapterInfo {
    std::string vendor;
    std::string architecture;
    std::string device;
    std::string description;
    uint32_t subgroupMinSize;
    uint32_t subgroupMaxSize;
    bool isFallbackAdapter;
};

}
