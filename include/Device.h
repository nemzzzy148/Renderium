//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once
#include <memory>

#include "Buffer.h"
#include "Pipeline.h"
#include "Queue.h"
#include "Result.h"
#include "Shader.h"
#include "Texture.h"

namespace rhi {
template<typename Api>
class DeviceImpl;
template<typename Api>
class AdapterImpl;
template<typename Api>
class SurfaceImpl;
}

namespace renderium {

struct PipelineLayoutDescriptor;
class PipelineLayout;
class ShaderModule;
struct ComputePipelineDescriptor;
class ComputePipeline;
struct RenderPipelineDescriptor;
class RenderPipeline;
class Queue;
class Surface;

struct DeviceDescriptor {
    ShadingLanguage shadingLanguage = ShadingLanguage::SLANG;
};

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

class Device {
public:
    Device() = delete;

    const SupportedFeatures features;
    const SupportedLimits limits;
    const AdapterInfo adapterInfo;

    [[nodiscard]] Queue queue() const;

    [[nodiscard]] Buffer createBuffer(const BufferDescriptor& descriptor) const;
    [[nodiscard]] Texture createTexture(const TextureDescriptor& descriptor) const;

    [[nodiscard]] ShaderModule createShaderModule(const ShaderModuleDescriptor& descriptor) const;
    // PipelineLayout createPipelineLayout(const PipelineLayoutDescriptor& descriptor);
    [[nodiscard]] RenderPipeline createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;
private:
    struct Impl {
        virtual ~Impl() = default;

        virtual SupportedFeatures requestFeatures() = 0;
        virtual SupportedLimits requestLimits() = 0;
        virtual AdapterInfo requestAdapterInfo() = 0;

        virtual Result<std::unique_ptr<Queue::Impl>, RequestError> createQueue() = 0;

        virtual std::unique_ptr<Buffer::Impl> createBuffer(const BufferDescriptor& descriptor) = 0;
        virtual std::unique_ptr<Texture::Impl> createTexture(const TextureDescriptor& descriptor) = 0;

        virtual shader::CompiledShader compileShader(const std::string& code) = 0;
        virtual std::unique_ptr<RenderPipeline::Impl>
            createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;
    };
    explicit Device(std::unique_ptr<Impl> impl, std::unique_ptr<Queue::Impl> queueImpl,
        const SupportedFeatures features, const SupportedLimits &limits, AdapterInfo adapterInfo) :
        features(features), limits(limits), adapterInfo(std::move(adapterInfo)),
        impl(std::move(impl)), queueImpl(std::move(queueImpl)) {}

    std::unique_ptr<Impl> impl;
    std::unique_ptr<Queue::Impl> queueImpl;

    template<typename Api>
    friend class rhi::DeviceImpl;
    template<typename Api>
    friend class rhi::SurfaceImpl;
    template<typename Api>
    friend class rhi::AdapterImpl;
    friend class Queue;
    friend class Adapter;
};

}

template<> struct utils::enumBitwiseOperations<renderium::SupportedFeatures> : std::true_type {};