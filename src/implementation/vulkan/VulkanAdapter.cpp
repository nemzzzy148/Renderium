//
// Created by Nemesis Verstraete on 30/09/2026.
//

#include "VulkanAdapter.h"

#include "VulkanInstance.h"
#include "../Error.h"

namespace rhi::vulkan {

vk::DeviceSize VulkanAdapter::getDeviceLocalMemory(const vk::raii::PhysicalDevice &physicalDevice) {
    const auto memoryProperties = physicalDevice.getMemoryProperties();
    vk::DeviceSize total = 0;
    for (const auto&[size, flags] : memoryProperties.memoryHeaps) {
        if (flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
            total += size;
        }
    }

    return total;
}

float VulkanAdapter::powerPreferenceFactor(const renderium::PowerPreference powerPreference, const bool gpu) {
    switch (powerPreference) {
        case renderium::PowerPreference::None: return 1.0f;
        case renderium::PowerPreference::HighPerformance: return gpu ? 2.0f : 0.5f;
        case renderium::PowerPreference::LowPower: return gpu ? 0.5f : 2.0f;
        default: return 1.0f;
    }
}

float VulkanAdapter::getPhysicalDeviceScore(const vk::raii::PhysicalDevice& physicalDevice,
                                            const AdapterOptions& options) {
    if (auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            std::ranges::none_of(queueFamilies, [](const auto& queueFamily)
                { return !!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics); })) {
        return 0;
                }
    if (options.surface != nullptr) {
        if (std::ranges::none_of(physicalDevice.enumerateDeviceExtensionProperties(), [](const auto& ext) {
            return strcmp(ext.extensionName, surfaceExtension) == 0;
        })) return 0;
    }

    // --- score ---
    float score = 1000;

    // - device type -
    const float gpuFactor = powerPreferenceFactor(options.powerPreference, true);
    const float cpuFactor = powerPreferenceFactor(options.powerPreference, false);
    switch (const vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
            physicalDeviceProperties.deviceType) {
                case vk::PhysicalDeviceType::eIntegratedGpu: score *= cpuFactor; break;
                case vk::PhysicalDeviceType::eDiscreteGpu: score *= gpuFactor; break;
                case vk::PhysicalDeviceType::eCpu: score *= cpuFactor; break;
                default: return 0;
            }

    // - video ram -
    const float vRam = static_cast<float>(getDeviceLocalMemory(physicalDevice)) / (1024.f * 1024.f * 1024.f);
    score+=std::log2f(vRam);

    return score;
}

VulkanAdapter::PhysicalDeviceResult VulkanAdapter::createPhysicalDevice(const VulkanInstance& instance,
                                                                        const AdapterOptions& options) {
    const std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesFound);
    int theChosenIndex = 0; float highestScore = 0;

    for (int i = 0; i < physicalDevices.size(); i++) {
        const float score = getPhysicalDeviceScore(physicalDevices[i], options);
        if (score <= highestScore) continue;
        highestScore = score;
        theChosenIndex = i;
    }
    if (highestScore == 0.0f) return PhysicalDeviceResult::err(VulkanError::NoPhysicalDevicesSuitable);
    return PhysicalDeviceResult::ok(physicalDevices[theChosenIndex]);
}

VulkanAdapter::QueueIndexResult VulkanAdapter::selectQueueFamilyIndex(const vk::QueueFlagBits flags,
    const vk::raii::PhysicalDevice &physicalDevice) {
    const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (flags & queueFamilyProperties[i].queueFlags) {
            return QueueIndexResult::ok(i);
        }
    }
    return QueueIndexResult::err(VulkanError::NoQueueFamilySuitable);
}
VulkanAdapter::AdapterResult VulkanAdapter::create(std::shared_ptr<VulkanInstance> instance, const AdapterOptions& options) {
    auto physicalDeviceResult = createPhysicalDevice(*instance, options);
    if (!physicalDeviceResult.isOk()) return AdapterResult::err(physicalDeviceResult.unwrapError());
    auto physicalDevice = physicalDeviceResult.unwrap();

    auto queueIndexResult = selectQueueFamilyIndex(vk::QueueFlagBits::eCompute, physicalDevice);
    if (!physicalDeviceResult.isOk()) return AdapterResult::err(queueIndexResult.unwrapError());

    return AdapterResult::ok(VulkanAdapter(std::move(instance),
        std::make_shared<vk::raii::PhysicalDevice>(std::move(physicalDevice)),
        options.surface, queueIndexResult.unwrap()));
}

uint32_t VulkanAdapter::getVulkanVersion() const {
    return std::min(physicalDevice->getProperties().apiVersion, instance->getVulkanVersion());
}

bool VulkanAdapter::hasExtension(const char* extensionName) const {
    return std::ranges::any_of(physicalDevice->enumerateDeviceExtensionProperties(), [extensionName](const auto& extension) {
        return strcmp(extension.extensionName.data(), extensionName) == 0;
    });
}

bool VulkanAdapter::hasFormatFeature(const vk::Format format, const vk::FormatFeatureFlags neededFlags) const {
    return (physicalDevice->getFormatProperties(format).optimalTilingFeatures & neededFlags) == neededFlags;
}

bool VulkanAdapter::hasAllFormats(const std::vector<vk::Format> &formats, const vk::FormatFeatureFlags neededFlags) const {
    return std::ranges::all_of(formats, [&](const auto& format) {
        return hasFormatFeature(format, neededFlags);
    });
}

bool VulkanAdapter::image3DSupported(const vk::Format format) const {
    vk::PhysicalDeviceImageFormatInfo2 info;

    // check format 3d by getting physical props of it in 3d "mode"
    info.format = format;
    info.type = vk::ImageType::e3D;
    info.tiling = vk::ImageTiling::eOptimal;
    info.usage = vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled;

    vk::ImageFormatProperties2 out;
    const vk::Result res = (**physicalDevice).getImageFormatProperties2(&info, &out);
    return res == vk::Result::eSuccess;
}

renderium::SupportedFeatures VulkanAdapter::requestFeatures() const {
    const auto version = getVulkanVersion();
    const auto f = physicalDevice->getFeatures();
    const auto structChain = physicalDevice->getFeatures2<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features>();
    const auto& f11 = structChain.get<vk::PhysicalDeviceVulkan11Features>();
    const auto& f12 = structChain.get<vk::PhysicalDeviceVulkan12Features>();
    const auto& f13 = structChain.get<vk::PhysicalDeviceVulkan13Features>();
    auto p = physicalDevice->getProperties();
    auto queueFamilyProps = physicalDevice->getQueueFamilyProperties();
    auto extensions = physicalDevice->enumerateDeviceExtensionProperties();

    renderium::SupportedFeatures features = {};

    if (version >= VK_API_VERSION_1_2 && f.shaderInt64 &&
        f12.shaderBufferInt64Atomics && f12.shaderSharedInt64Atomics)
        features |= renderium::SupportedFeatures::AtomicVec2uMinMax;

    if (version >= VK_API_VERSION_1_3 && f.robustBufferAccess &&
        f.fullDrawIndexUint32 && f.imageCubeArray && f.independentBlend &&
        f.sampleRateShading && f.fragmentStoresAndAtomics &&
        f13.dynamicRendering && f13.synchronization2) // dynamic render and sync 2 is renderiums baseline.
            features |= renderium::SupportedFeatures::CoreFeaturesAndLimits;

    if (f.depthClamp) features |= renderium::SupportedFeatures::DepthClipControl;

    if (hasFormatFeature(vk::Format::eD32SfloatS8Uint,
        vk::FormatFeatureFlagBits::eSampledImage | vk::FormatFeatureFlagBits::eDepthStencilAttachment)) {
        features |= renderium::SupportedFeatures::Depth32FloatStencil8;
        }

    if (f.textureCompressionBC) features |= renderium::SupportedFeatures::TextureCompressionBC;
    static constexpr vk::Format bcFormats[] = {
        vk::Format::eBc1RgbaUnormBlock, vk::Format::eBc1RgbaSrgbBlock,
        vk::Format::eBc2UnormBlock, vk::Format::eBc2SrgbBlock,
        vk::Format::eBc3UnormBlock, vk::Format::eBc3SrgbBlock,
        vk::Format::eBc4UnormBlock, vk::Format::eBc4SnormBlock,
        vk::Format::eBc5UnormBlock, vk::Format::eBc5SnormBlock,
        vk::Format::eBc6HUfloatBlock, vk::Format::eBc7UnormBlock, vk::Format::eBc7SrgbBlock
    };
    if (std::ranges::all_of(bcFormats, [&](const auto& format) {
        return image3DSupported(format);
    })) features |= renderium::SupportedFeatures::TextureCompressionBCSliced3D;

    if (f.textureCompressionETC2) features |= renderium::SupportedFeatures::TextureCompressionETC2;
    if (f.textureCompressionASTC_LDR) features |= renderium::SupportedFeatures::TextureCompressionASTC;
    static std::vector astcFormat = {
        vk::Format::eAstc4x4UnormBlock, vk::Format::eAstc4x4SrgbBlock,
        vk::Format::eAstc5x4UnormBlock, vk::Format::eAstc5x4SrgbBlock,
        vk::Format::eAstc5x5UnormBlock, vk::Format::eAstc5x5SrgbBlock,
        vk::Format::eAstc6x5UnormBlock, vk::Format::eAstc6x5SrgbBlock,
        vk::Format::eAstc8x5UnormBlock, vk::Format::eAstc8x5SrgbBlock,
        vk::Format::eAstc8x6UnormBlock, vk::Format::eAstc8x6SrgbBlock,
        vk::Format::eAstc8x8UnormBlock, vk::Format::eAstc8x8SrgbBlock,
        vk::Format::eAstc10x5UnormBlock, vk::Format::eAstc10x5SrgbBlock,
        vk::Format::eAstc10x6UnormBlock, vk::Format::eAstc10x6SrgbBlock,
        vk::Format::eAstc10x8UnormBlock, vk::Format::eAstc10x8SrgbBlock,
        vk::Format::eAstc10x10UnormBlock, vk::Format::eAstc10x10SrgbBlock,
        vk::Format::eAstc12x10UnormBlock, vk::Format::eAstc12x10SrgbBlock,
        vk::Format::eAstc12x12UnormBlock, vk::Format::eAstc12x12SrgbBlock
    };
    if (std::ranges::all_of(astcFormat, [&](const auto& format) {
        return image3DSupported(format);
    })) features |= renderium::SupportedFeatures::TextureCompressionASTCSliced3D;

    const auto queueProp = physicalDevice->getQueueFamilyProperties()[queueFamilyIndex];
    if (p.limits.timestampPeriod > 0.0f && p.limits.timestampComputeAndGraphics && queueProp.timestampValidBits > 0)
        features |= renderium::SupportedFeatures::TimestampQuery;

    if (f.drawIndirectFirstInstance) features |= renderium::SupportedFeatures::IndirectFirstInstance;

    if (f12.shaderFloat16 && f11.storageBuffer16BitAccess && f11.uniformAndStorageBuffer16BitAccess)
        features |= renderium::SupportedFeatures::ShaderF16;

    if (hasFormatFeature(vk::Format::eB10G11R11UfloatPack32, vk::FormatFeatureFlagBits::eColorAttachment |
        vk::FormatFeatureFlagBits::eColorAttachmentBlend | vk::FormatFeatureFlagBits::eSampledImage))
        features |= renderium::SupportedFeatures::RG11B10UfloatRenderable;

    if (hasFormatFeature(vk::Format::eB8G8R8A8Unorm, vk::FormatFeatureFlagBits::eSampledImage |
        vk::FormatFeatureFlagBits::eStorageImage)) features |= renderium::SupportedFeatures::BGRA8UnormStorage;

    static std::vector f32 = {
        vk::Format::eR32Sfloat, vk::Format::eR32G32Sfloat, vk::Format::eR32G32B32Sfloat, vk::Format::eR32G32B32A32Sfloat};
    if (hasAllFormats(f32, vk::FormatFeatureFlagBits::eSampledImage |
        vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) features |= renderium::SupportedFeatures::Float32Filterable;
    if (hasAllFormats(f32, vk::FormatFeatureFlagBits::eColorAttachment |
        vk::FormatFeatureFlagBits::eColorAttachmentBlend)) features |= renderium::SupportedFeatures::Float32Blendable;


    return features;
}

renderium::SupportedLimits VulkanAdapter::requestLimits() const {
    const auto features = physicalDevice->getFeatures();
    const auto limits = physicalDevice->getProperties().limits;
    const auto version = getVulkanVersion();

    uint64_t maxBufferSize = 268435456; // 256 MiB default
    uint32_t maxBindingsPerBindGroup = 0;

    if (version < VK_API_VERSION_1_1) {
        maxBindingsPerBindGroup = std::min({limits.maxPerStageDescriptorSampledImages,
        limits.maxPerStageDescriptorSamplers, limits.maxPerStageDescriptorStorageBuffers,
        limits.maxPerStageDescriptorStorageImages, limits.maxPerStageDescriptorUniformBuffers,
        limits.maxPerStageDescriptorInputAttachments});
    }

    if (version >= VK_API_VERSION_1_1) {
        const auto structChain = physicalDevice->getProperties2<vk::PhysicalDeviceProperties2,
            vk::PhysicalDeviceMaintenance3Properties, vk::PhysicalDeviceMaintenance4Properties>();

        const auto extensions = physicalDevice->enumerateDeviceExtensionProperties();
        const bool hasMaintenance4 = hasExtension(vk::KHRMaintenance4ExtensionName);

        maxBufferSize = hasMaintenance4 ? structChain.get<vk::PhysicalDeviceMaintenance4Properties>().maxBufferSize :
            structChain.get<vk::PhysicalDeviceMaintenance3Properties>().maxMemoryAllocationSize;

        maxBindingsPerBindGroup = std::min(
            structChain.get<vk::PhysicalDeviceMaintenance3Properties>().maxPerSetDescriptors, 1000u);
        // clamp with web gpu default
    }

    return {
        .maxTextureDimension1D = limits.maxImageDimension1D,
        .maxTextureDimension2D = limits.maxImageDimension2D,
        .maxTextureDimension3D = limits.maxImageDimension3D,
        .maxTextureArrayLayers = limits.maxImageArrayLayers,
        .maxBindGroups = limits.maxBoundDescriptorSets,
        .maxBindGroupsPlusVertexBuffers = limits.maxBoundDescriptorSets + limits.maxVertexInputBindings,
        .maxImmediateSize = limits.maxPushConstantsSize,
        .maxBindingsPerBindGroup = maxBindingsPerBindGroup,
        .maxDynamicUniformBuffersPerPipelineLayout = limits.maxDescriptorSetUniformBuffersDynamic,
        .maxDynamicStorageBuffersPerPipelineLayout = limits.maxDescriptorSetStorageBuffersDynamic,
        .maxSampledTexturesPerShaderStage = limits.maxPerStageDescriptorSampledImages,
        .maxSamplersPerShaderStage = limits.maxPerStageDescriptorSamplers,
        .maxStorageBuffersPerShaderStage = limits.maxPerStageDescriptorStorageBuffers,
        .maxStorageBuffersInVertexStage = features.vertexPipelineStoresAndAtomics ?
            std::min(8u, limits.maxPerStageDescriptorStorageBuffers) : 0u,
        .maxStorageBuffersInFragmentStage = features.fragmentStoresAndAtomics ?
            std::min(8u, limits.maxPerStageDescriptorStorageBuffers) : 0u,
        // If false, when in the vertex or fragment stage, the shader can't write to (atomic) storage buffer.
        // So then the user wouldn't be able to access storage buffers.
        // Take the minimum of 8 and the maximum amount of per stage(vertex, fragment, ...) storage buffers.
        // Same logic with the textures below.
        .maxStorageTexturesPerShaderStage = limits.maxPerStageDescriptorStorageImages,
        .maxStorageTexturesInVertexStage = features.vertexPipelineStoresAndAtomics ?
            std::min(4u, limits.maxPerStageDescriptorStorageImages) : 0u,
        .maxStorageTexturesInFragmentStage = features.fragmentStoresAndAtomics ?
            std::min(4u, limits.maxPerStageDescriptorStorageImages) : 0u,

        .maxUniformBuffersPerShaderStage = limits.maxPerStageDescriptorUniformBuffers,
        .maxUniformBufferBindingSize = limits.maxUniformBufferRange,
        .maxStorageBufferBindingSize = limits.maxStorageBufferRange,
        .minUniformBufferOffsetAlignment = static_cast<uint32_t>(limits.minUniformBufferOffsetAlignment),
        .minStorageBufferOffsetAlignment = static_cast<uint32_t>(limits.minStorageBufferOffsetAlignment),
        .maxVertexBuffers = limits.maxVertexInputBindings,
        .maxBufferSize = maxBufferSize,
        .maxVertexAttributes = limits.maxVertexInputAttributes,
        .maxVertexBufferArrayStride = limits.maxVertexInputBindingStride,
        .maxInterStageShaderVariables =
            std::min(limits.maxVertexOutputComponents, limits.maxFragmentInputComponents) / 4,
        /*
         * The limit defines the max amount of individual components.
         * Because each variable (like a color or vector) contains up to 4 components (e.g., R, G, B, A),
         * we divide by 4 to get the maximum number of variables.
         */
        .maxColorAttachments = limits.maxColorAttachments,
        .maxColorAttachmentBytesPerSample = 32, // no direct limit, web gpu default is 32
        .maxComputeWorkgroupStorageSize = limits.maxComputeSharedMemorySize,
        .maxComputeInvocationsPerWorkgroup = limits.maxComputeWorkGroupInvocations,
        .maxComputeWorkgroupSizeX = limits.maxComputeWorkGroupSize[0],
        .maxComputeWorkgroupSizeY = limits.maxComputeWorkGroupSize[1],
        .maxComputeWorkgroupSizeZ = limits.maxComputeWorkGroupSize[2],
        .maxComputeWorkgroupsPerDimension = std::ranges::min(limits.maxComputeWorkGroupCount),
    };
}

renderium::AdapterInfo VulkanAdapter::requestAdapterInfo() const {
    const auto properties = physicalDevice->getProperties();
    const auto version = getVulkanVersion();
    renderium::AdapterInfo info;
    
    switch (properties.vendorID) {
        case 0x1002: info.vendor = "amd"; break;
        case 0x10DE: info.vendor = "nvidia"; break;
        case 0x8086: info.vendor = "intel"; break;
        case 0x13B5: info.vendor = "arm"; break;
        case 0x5143: info.vendor = "qualcomm"; break;
        case 0x1010: info.vendor = "img-tec"; break;
        default: info.vendor = ""; break;
    }
    info.architecture = "";
    info.device = std::string(properties.deviceName.data());

    if (version >= VK_VERSION_1_2 || hasExtension(vk::KHRDriverPropertiesExtensionName)) {
        const auto driverProps = physicalDevice->getProperties2<vk::PhysicalDeviceProperties2,
            vk::PhysicalDeviceDriverProperties>().get<vk::PhysicalDeviceDriverProperties>();
        info.description = std::string(driverProps.driverName.data()) + " " + driverProps.driverInfo.data();
    } else {
        info.description = info.device;
    }

    // Default sub group sizes
    info.subgroupMinSize = 4;
    info.subgroupMaxSize = 128;

    if (version >= VK_VERSION_1_3 || hasExtension(vk::EXTSubgroupSizeControlExtensionName)) {
        const auto subGroupProps = physicalDevice->getProperties2<vk::PhysicalDeviceProperties2,
            vk::PhysicalDeviceSubgroupSizeControlProperties>().get<vk::PhysicalDeviceSubgroupSizeControlProperties>();
        info.subgroupMaxSize = subGroupProps.maxSubgroupSize;
        info.subgroupMinSize = subGroupProps.minSubgroupSize;
    } else if (version >= VK_VERSION_1_1) {
        const auto subGroupProps = physicalDevice->getProperties2<vk::PhysicalDeviceProperties2,
            vk::PhysicalDeviceSubgroupProperties>().get<vk::PhysicalDeviceSubgroupProperties>();
        info.subgroupMaxSize = info.subgroupMinSize = subGroupProps.subgroupSize;
    }

    info.isFallbackAdapter = properties.deviceType == vk::PhysicalDeviceType::eCpu;
    return std::move(info);
}

VulkanAdapter::DeviceResult VulkanAdapter::requestDevice(const renderium::DeviceDescriptor& descriptor) const {
    return VulkanDevice::create(instance, physicalDevice, descriptor, surfaceSupport, queueFamilyIndex);
}

}