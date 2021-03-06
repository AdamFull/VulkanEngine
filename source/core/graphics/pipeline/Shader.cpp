#include "Shader.h"
#include "util/ulog.hpp"
#include "graphics/VulkanHighLevel.h"
#include "ShaderCache.h"
#include "filesystem/FilesystemHelper.h"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>

//Spirv cross reflection doc
//https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide
//https://github.com/KhronosGroup/SPIRV-Cross/blob/master/main.cpp

using namespace engine;
using namespace engine::core::pipeline;

namespace spirv_cross
{
    BufferPackingStandard CompilerGLSLExt::get_packing_standart(const SPIRType& type)
    {
        if(buffer_is_packing_standard(type, BufferPackingStd140))
            return BufferPackingStd140;
        else if(buffer_is_packing_standard(type, BufferPackingStd430))
            return BufferPackingStd430;
        else if(buffer_is_packing_standard(type, BufferPackingStd140EnhancedLayout))
            return BufferPackingStd140EnhancedLayout;
        else if(buffer_is_packing_standard(type, BufferPackingStd430EnhancedLayout))
            return BufferPackingStd430EnhancedLayout;
        else if(buffer_is_packing_standard(type, BufferPackingHLSLCbuffer))
            return BufferPackingHLSLCbuffer;
        else if(buffer_is_packing_standard(type, BufferPackingHLSLCbufferPackOffset))
            return BufferPackingHLSLCbufferPackOffset;
        else if(buffer_is_packing_standard(type, BufferPackingScalar))
            return BufferPackingScalar;
        else if(buffer_is_packing_standard(type, BufferPackingScalarEnhancedLayout))
            return BufferPackingScalarEnhancedLayout;
        return BufferPackingStd140;
    }

    uint32_t CompilerGLSLExt::get_packed_base_size(const SPIRType &type)
    {
        auto packing = get_packing_standart(type);
        return type_to_packed_base_size(type, packing);
    }

    uint32_t CompilerGLSLExt::get_packed_alignment(const SPIRType &type, const Bitset &flags)
    {
        auto packing = get_packing_standart(type);
        return type_to_packed_alignment(type, flags, packing);
    }

    uint32_t CompilerGLSLExt::get_packed_array_stride(const SPIRType &type, const Bitset &flags)
    {
        auto packing = get_packing_standart(type);
        return type_to_packed_array_stride(type, flags, packing);
    }

    uint32_t CompilerGLSLExt::get_packed_size(const SPIRType &type, const Bitset &flags)
    {
        auto packing = get_packing_standart(type);
        return type_to_packed_size(type, flags, packing);
    }
}

class CShaderIncluder : public glslang::TShader::Includer 
{
public:
	IncludeResult *includeLocal(const char *headerName, const char *includerName, size_t inclusionDepth) override 
    {
        fs::path local_path {};
        if(inclusionDepth == 1)
        {
            directory = fs::path(includerName).parent_path();
            local_path = directory;
        }
        else
        {
            directory = directory / fs::path(includerName).parent_path();
            local_path = directory;
        }

        local_path = fs::weakly_canonical(directory / headerName);
		auto fileLoaded = FilesystemHelper::readFile(local_path.string());

		if (fileLoaded.empty()) 
        {
			std::stringstream ss;
			ss << "In shader file: " << includerName << " Shader Include could not be loaded: " << std::quoted(headerName);
			utl::logger::log(utl::ELogLevel::eError, ss.str());
			return nullptr;
		}

		auto content = new char[fileLoaded.size()];
		std::memcpy(content, fileLoaded.data(), fileLoaded.size());
		return new IncludeResult(headerName, content, fileLoaded.size(), content);
	}

	IncludeResult *includeSystem(const char *headerName, const char *includerName, size_t inclusionDepth) override 
    {
        auto header = fs::path("shaders") / headerName;
		auto fileLoaded = FilesystemHelper::readFile(header);

		if (fileLoaded.empty()) {
			std::stringstream ss;
			ss << "Shader Include could not be loaded: " << std::quoted(headerName);
			utl::logger::log(utl::ELogLevel::eError, ss.str());
			return nullptr;
		}

		auto content = new char[fileLoaded.size()];
		std::memcpy(content, fileLoaded.data(), fileLoaded.size());
		return new IncludeResult(headerName, content, fileLoaded.size(), content);
	}

	void releaseInclude(IncludeResult *result) override 
    {
		if (result) 
        {
			delete[] static_cast<char *>(result->userData);
			delete result;
		}
	}
private:
    fs::path directory{""};
};

EShLanguage getEshLanguage(vk::ShaderStageFlagBits stageFlag) 
{
	switch (stageFlag) {
	case vk::ShaderStageFlagBits::eCompute:
		return EShLangCompute;
	case vk::ShaderStageFlagBits::eVertex:
		return EShLangVertex;
	case vk::ShaderStageFlagBits::eTessellationControl:
		return EShLangTessControl;
	case vk::ShaderStageFlagBits::eTessellationEvaluation:
		return EShLangTessEvaluation;
	case vk::ShaderStageFlagBits::eGeometry:
		return EShLangGeometry;
	case vk::ShaderStageFlagBits::eFragment:
		return EShLangFragment;
	default:
		return EShLangCount;
	}
}

TBuiltInResource getResources() 
{
    //auto props = UDevice->GetPhysical().getProperties();
	TBuiltInResource resources = {};
	resources.maxLights                                 = 32;
    resources.maxClipPlanes                             = 6;
    resources.maxTextureUnits                           = 32;
    resources.maxTextureCoords                          = 32;
    resources.maxVertexAttribs                          = 64;
    resources.maxVertexUniformComponents                = 4096;
    resources.maxVaryingFloats                          = 64;
    resources.maxVertexTextureImageUnits                = 32;
    resources.maxCombinedTextureImageUnits              = 80;
    resources.maxTextureImageUnits                      = 32;
    resources.maxFragmentUniformComponents              = 4096;
    resources.maxDrawBuffers                            = 32;
    resources.maxVertexUniformVectors                   = 128;
    resources.maxVaryingVectors                         = 8;
    resources.maxFragmentUniformVectors                 = 16;
    resources.maxVertexOutputVectors                    = 16;
    resources.maxFragmentInputVectors                   = 15;
    resources.minProgramTexelOffset                     = -8;
    resources.maxProgramTexelOffset                     = 7;
    resources.maxClipDistances                          = 8;
    resources.maxComputeWorkGroupCountX                 = 65535;
    resources.maxComputeWorkGroupCountY                 = 65535;
    resources.maxComputeWorkGroupCountZ                 = 65535;
    resources.maxComputeWorkGroupSizeX                  = 1024;
    resources.maxComputeWorkGroupSizeY                  = 1024;
    resources.maxComputeWorkGroupSizeZ                  = 64;
    resources.maxComputeUniformComponents               = 1024;
    resources.maxComputeTextureImageUnits               = 16;
    resources.maxComputeImageUniforms                   = 8;
    resources.maxComputeAtomicCounters                  = 8;
    resources.maxComputeAtomicCounterBuffers            = 1;
    resources.maxVaryingComponents                      = 60;
    resources.maxVertexOutputComponents                 = 64;
    resources.maxGeometryInputComponents                = 64;
    resources.maxGeometryOutputComponents               = 128;
    resources.maxFragmentInputComponents                = 128;
    resources.maxImageUnits                             = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs   = 8;
    resources.maxCombinedShaderOutputResources          = 8;
    resources.maxImageSamples                           = 0;
    resources.maxVertexImageUniforms                    = 0;
    resources.maxTessControlImageUniforms               = 0;
    resources.maxTessEvaluationImageUniforms            = 0;
    resources.maxGeometryImageUniforms                  = 0;
    resources.maxFragmentImageUniforms                  = 8;
    resources.maxCombinedImageUniforms                  = 8;
    resources.maxGeometryTextureImageUnits              = 16;
    resources.maxGeometryOutputVertices                 = 256;
    resources.maxGeometryTotalOutputComponents          = 1024;
    resources.maxGeometryUniformComponents              = 1024;
    resources.maxGeometryVaryingComponents              = 64;
    resources.maxTessControlInputComponents             = 128;
    resources.maxTessControlOutputComponents            = 128;
    resources.maxTessControlTextureImageUnits           = 16;
    resources.maxTessControlUniformComponents           = 1024;
    resources.maxTessControlTotalOutputComponents       = 4096;
    resources.maxTessEvaluationInputComponents          = 128;
    resources.maxTessEvaluationOutputComponents         = 128;
    resources.maxTessEvaluationTextureImageUnits        = 16;
    resources.maxTessEvaluationUniformComponents        = 1024;
    resources.maxTessPatchComponents                    = 120;
    resources.maxPatchVertices                          = 32;
    resources.maxTessGenLevel                           = 64;
    resources.maxViewports                              = 16;
    resources.maxVertexAtomicCounters                   = 0;
    resources.maxTessControlAtomicCounters              = 0;
    resources.maxTessEvaluationAtomicCounters           = 0;
    resources.maxGeometryAtomicCounters                 = 0;
    resources.maxFragmentAtomicCounters                 = 8;
    resources.maxCombinedAtomicCounters                 = 8;
    resources.maxAtomicCounterBindings                  = 1;
    resources.maxVertexAtomicCounterBuffers             = 0;
    resources.maxTessControlAtomicCounterBuffers        = 0;
    resources.maxTessEvaluationAtomicCounterBuffers     = 0;
    resources.maxGeometryAtomicCounterBuffers           = 0;
    resources.maxFragmentAtomicCounterBuffers           = 1;
    resources.maxCombinedAtomicCounterBuffers           = 1;
    resources.maxAtomicCounterBufferSize                = 16384;
    resources.maxTransformFeedbackBuffers               = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances                          = 8;
    resources.maxCombinedClipAndCullDistances           = 8;
    resources.maxSamples                                = 4;
    resources.maxMeshOutputVerticesNV                   = 256;
    resources.maxMeshOutputPrimitivesNV                 = 512;
    resources.maxMeshWorkGroupSizeX_NV                  = 32;
    resources.maxMeshWorkGroupSizeY_NV                  = 1;
    resources.maxMeshWorkGroupSizeZ_NV                  = 1;
    resources.maxTaskWorkGroupSizeX_NV                  = 32;
    resources.maxTaskWorkGroupSizeY_NV                  = 1;
    resources.maxTaskWorkGroupSizeZ_NV                  = 1;
    resources.maxMeshViewCountNV                        = 4;

	resources.limits.nonInductiveForLoops               = true;
	resources.limits.whileLoops                         = true;
	resources.limits.doWhileLoops                       = true;
	resources.limits.generalUniformIndexing             = true;
	resources.limits.generalAttributeMatrixVectorIndexing = true;
	resources.limits.generalVaryingIndexing             = true;
	resources.limits.generalSamplerIndexing             = true;
	resources.limits.generalVariableIndexing            = true;
	resources.limits.generalConstantMatrixVectorIndexing = true;
	return resources;
}

CShader::~CShader()
{
    for(auto& stage : vShaderModules)
        UDevice->destroy(&stage.module);

    vShaderModules.clear();
    vShaderStage.clear();
    mUniforms.clear();
    mUniformBlocks.clear();
    mInputAttributes.clear();
    mOutputAttributes.clear();
    mConstants.clear();
    //Destroy before clear
    vDescriptorSetLayouts.clear();
    vDescriptorPools.clear();
    lastDescriptorBinding = 0;
}

void CShader::addStage(const std::filesystem::path &moduleName, const std::string& moduleCode, const std::string &preamble)
{
    std::vector<uint32_t> spirv;
    auto spirv_cache = CShaderCache::inst()->get(moduleName.filename().string(), moduleCode);
    if (!spirv_cache)
    {
        vShaderStage.emplace_back(getShaderStage(moduleName));
        auto language = getEshLanguage(vShaderStage.back());
        glslang::TProgram program;
        glslang::TShader shader(language);
        auto resources = getResources();

        auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);

        auto srCode = std::string(moduleCode.begin(), moduleCode.end());

        auto shaderName = moduleName.string();
        auto shaderNameCstr = shaderName.c_str();
        auto shaderSource = srCode.c_str();
        shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, &shaderNameCstr, 1);
        shader.setPreamble(preamble.c_str());

        auto defaultVersion = glslang::EShTargetVulkan_1_1;
        shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 110);
        shader.setEnvClient(glslang::EShClientVulkan, defaultVersion);
        shader.setEnvTarget(glslang::EShTargetSpv, UDevice->getVulkanVersion() >= VK_API_VERSION_1_1 ? glslang::EShTargetSpv_1_3 : glslang::EShTargetSpv_1_0);

        CShaderIncluder includer;

        std::string str;

        if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &str, includer))
        {
            std::stringstream ss;
            ss << "\n****************SHADER_PREPROCESSING_FAILED****************\n";
            ss << "Shader: " << moduleName << "\n";
            ss << shader.getInfoLog() << "\n";
            ss << shader.getInfoDebugLog() << "\n";
            ss << "***********************************************************";
            utl::logger::log(utl::ELogLevel::eError, ss.str());
            assert(false && "SPRIV shader preprocess failed!");
        }

        if (!shader.parse(&resources, defaultVersion, true, messages, includer))
        {
            std::stringstream ss;
            ss << "\n*******************SHADER_PARSING_FAILED*******************\n";
            ss << "Shader: " << moduleName << "\n";
            ss << shader.getInfoLog() << "\n";
            ss << shader.getInfoDebugLog() << "\n";
            ss << "***********************************************************";
            utl::logger::log(utl::ELogLevel::eError, ss.str());
            assert(false && "SPRIV shader parse failed!");
        }

        program.addShader(&shader);

        if (!program.link(messages) || !program.mapIO())
        {
            std::stringstream ss;
            ss << "\n*******************SHADER_LINKING_FAILED*******************\n";
            ss << "Shader: " << moduleName << "\n";
            ss << shader.getInfoLog() << "\n";
            ss << shader.getInfoDebugLog() << "\n";
            ss << "***********************************************************";
            utl::logger::log(utl::ELogLevel::eError, ss.str());
            assert(false && "Error while linking shader program.");
        }

        if (!program.buildReflection())
        {
            std::stringstream ss;
            ss << "\n***************SHADER_BUILD_REFLECTION_FAILED**************\n";
            ss << "Shader: " << moduleName << "\n";
            ss << shader.getInfoLog() << "\n";
            ss << shader.getInfoDebugLog() << "\n";
            ss << "***********************************************************";
            utl::logger::log(utl::ELogLevel::eError, ss.str());
            assert(false && "Error while building shader reflection.");
        }

        glslang::SpvOptions spvOptions;
#if defined(_DEBUG)
        spvOptions.generateDebugInfo = true;
        spvOptions.disableOptimizer = true;
        spvOptions.optimizeSize = false;
#else
        spvOptions.generateDebugInfo = false;
        spvOptions.disableOptimizer = false;
        spvOptions.optimizeSize = true;
#endif

        spv::SpvBuildLogger logger;
        GlslangToSpv(*program.getIntermediate(static_cast<EShLanguage>(language)), spirv, &logger, &spvOptions);
        CShaderCache::inst()->add(moduleName.filename().string(), getShaderStage(moduleName), spirv, moduleCode);
    }
    else
    {
        vShaderStage.emplace_back(spirv_cache.value().shaderStage);
        spirv = spirv_cache.value().shaderCode;
    }

    buildReflection(spirv, vShaderStage.back());

    createShaderModule(spirv);
}

void CShader::buildSpecializationInfo()
{
    //TODO: finish this
    auto& currentStage = vShaderStage.back();

    //Constants should be same type
    for(auto& [name, constant] : mConstants)
    {
        if(constant.stageFlags & currentStage)
        {
            vk::SpecializationMapEntry mapEntry{};
            mapEntry.constantID = constant.constantId;
            mapEntry.offset = 0;
            mapEntry.size = constant.size;
            mMapEntries[currentStage].emplace_back(std::move(mapEntry));

            vk::SpecializationInfo specializationInfo{};
            specializationInfo.mapEntryCount = 1;
            specializationInfo.pMapEntries = (--mMapEntries.end())->second.data();
            specializationInfo.dataSize = constant.size;
            //mSpecializationInfo.emplace(currentStage, std::move(specializationInfo));
        }
    }
}

void CShader::createShaderModule(const std::vector<uint32_t>& spirv)
{
    auto& currentStage = vShaderStage.back();

    try
    {
        vk::ShaderModuleCreateInfo shaderCI{};
        shaderCI.codeSize = spirv.size() * sizeof(uint32_t);
        shaderCI.pCode = spirv.data();

        vk::ShaderModule shaderModule{VK_NULL_HANDLE};
        vk::Result res = UDevice->create(shaderCI, &shaderModule);
        assert(res == vk::Result::eSuccess && "Cannot create shader module.");

        vk::PipelineShaderStageCreateInfo shaderStageCI{};
        shaderStageCI.stage = currentStage;
        shaderStageCI.module = shaderModule;
        shaderStageCI.pName = "main";
        
        auto specInfo = mSpecializationInfo.find(currentStage);
        if(specInfo != mSpecializationInfo.end())
            shaderStageCI.pSpecializationInfo = &specInfo->second;

        vShaderModules.emplace_back(std::move(shaderStageCI));
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create shader module!");
    }
}

void CShader::finalizeReflection()
{
    std::map<vk::DescriptorType, uint32_t> mDescriptorCounter;
    //Preparing descriptors for uniform/storage buffers
    for (const auto &[uniformBlockName, uniformBlock] : mUniformBlocks) 
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniformBlock.binding;
        descriptorSetLayoutBinding.descriptorType = uniformBlock.descriptorType;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniformBlock.stageFlags;
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);
        mDescriptorCounter[uniformBlock.descriptorType]++;
    }

    for (const auto &[uniformName, uniform] : mUniforms) 
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniform.binding;
        descriptorSetLayoutBinding.descriptorType = uniform.descriptorType;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniform.stageFlags;
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);
        mDescriptorCounter[uniform.descriptorType]++;
    }

    for(auto& [descriptorType, count] : mDescriptorCounter)
        vDescriptorPools.emplace_back(vk::DescriptorPoolSize{descriptorType, count * descriptorMultiplier});

    // Sort descriptors by binding.
	std::sort(vDescriptorSetLayouts.begin(), vDescriptorSetLayouts.end(), 
    [](const vk::DescriptorSetLayoutBinding &l, const vk::DescriptorSetLayoutBinding &r) 
    {
		return l.binding < r.binding;
	});

    // Gets the last descriptors binding.
	if (!vDescriptorSetLayouts.empty())
		lastDescriptorBinding = vDescriptorSetLayouts.back().binding;
}

std::optional<CUniform> CShader::getUniform(const std::string &name) const
{
    if (auto it = mUniforms.find(name); it != mUniforms.end())
		return it->second;
	return std::nullopt;
}

std::optional<CUniformBlock> CShader::getUniformBlock(const std::string &name) const
{
    if (auto it = mUniformBlocks.find(name); it != mUniformBlocks.end())
		return it->second;
	return std::nullopt;
}

std::optional<CPushConstBlock> CShader::getPushBlock(const std::string &name) const
{
    if (auto it = mPushBlocks.find(name); it != mPushBlocks.end())
		return it->second;
	return std::nullopt;
}

std::optional<CConstant> CShader::getConstant(const std::string &name) const
{
    if (auto it = mConstants.find(name); it != mConstants.end())
		return it->second;
	return std::nullopt;
}

std::optional<CAttribute> CShader::getInputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const
{
    if (auto it = mInputAttributes.find(stage); it != mInputAttributes.end())
    {
        if(auto _it = it->second.find(name); _it != it->second.end())
            return _it->second;
    }
	return std::nullopt;
}

std::optional<CAttribute> CShader::getOutputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const
{
	if (auto it = mOutputAttributes.find(stage); it != mOutputAttributes.end())
    {
        if(auto _it = it->second.find(name); _it != it->second.end())
            return _it->second;
    }
	return std::nullopt;
}

std::vector<vk::PushConstantRange> CShader::getPushConstantRanges() const
{
    std::vector<vk::PushConstantRange> pushConstantRanges;
	uint32_t currentOffset = 0;

	for (const auto &[pushBlockName, pushBlock] : mPushBlocks) 
    {
		vk::PushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = pushBlock.stageFlags;
		pushConstantRange.offset = currentOffset;
		pushConstantRange.size = static_cast<uint32_t>(pushBlock.size);
		pushConstantRanges.emplace_back(pushConstantRange);
		currentOffset += pushConstantRange.size;
	}
	return pushConstantRanges;
}

const std::optional<std::unordered_map<std::string, CAttribute>> CShader::getInputAttributes(vk::ShaderStageFlagBits stage) const
{
    if (auto it = mInputAttributes.find(stage); it != mInputAttributes.end())
        return it->second;
    return std::nullopt;
}

const std::optional<std::unordered_map<std::string, CAttribute>> CShader::getOutputAttributes(vk::ShaderStageFlagBits stage) const
{
    if (auto it = mOutputAttributes.find(stage); it != mOutputAttributes.end())
        return it->second;
    return std::nullopt;
}

const std::vector<vk::VertexInputAttributeDescription> CShader::getAttributeDescriptions(vk::ShaderStageFlagBits stage) const
{
    std::vector<vk::VertexInputAttributeDescription> attributes{};
    if (auto it = mInputAttributes.find(stage); it != mInputAttributes.end())
    {
        for(auto& [name, attribute] : it->second)
        {
            vk::VertexInputAttributeDescription desc{};
            desc.binding = attribute.set;
            desc.location = attribute.location;
            desc.offset = attribute.offset;
            desc.format = attribute.type;
            attributes.emplace_back(desc);
        }
    }
    return attributes;
}

const vk::VertexInputBindingDescription CShader::getBindingDescription(vk::ShaderStageFlagBits stage) const
{
    size_t size{0};
    vk::VertexInputBindingDescription desc{};
    desc.binding = 0;
    desc.inputRate = stage == vk::ShaderStageFlagBits::eVertex ? vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance;
    if (auto it = mInputAttributes.find(stage); it != mInputAttributes.end())
        for(auto& [name, attribute] : it->second)
            size += attribute.offset;
    desc.stride = size;
    return desc;
}
//Position from depth

void CShader::buildReflection(std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag)
{
    spirv_cross::CompilerGLSLExt compiler(spirv);
    auto active = compiler.get_active_interface_variables();
    auto resources = compiler.get_shader_resources(active);
    compiler.set_enabled_interface_variables(move(active));

    auto entry_points = compiler.get_entry_points_and_stages();
    for(auto& entry : entry_points)
    {

    }

    auto& execution_modes = compiler.get_execution_mode_bitset();
    execution_modes.for_each_bit([&](uint32_t i)
    {
        auto mode = static_cast<spv::ExecutionMode>(i);
        switch (mode)
		{
            case spv::ExecutionMode::ExecutionModeInvocations:
            {
                executionModeInvocations = compiler.get_execution_mode_argument(mode, 0);
            } break;
            case spv::ExecutionMode::ExecutionModeLocalSize: {
                for (uint32_t dim = 0; dim < 3; ++dim)
                    localSizes[dim] = compiler.get_execution_mode_argument(mode, dim);
            } break;
            case spv::ExecutionMode::ExecutionModeOutputVertices: {
                executionModeOutputVertices = compiler.get_execution_mode_argument(mode, 0);
            } break;
        }
    });
    
    //Parsing uniform buffers
    for(const auto& res : resources.uniform_buffers)
    {
        //Looking for the block if already exists and put stage flag
        auto it = mUniformBlocks.find(res.name);
        if (it != mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eUniformBuffer));
    }

    for(const auto& res : resources.storage_buffers)
    {
        auto it = mUniformBlocks.find(res.name);
        if(it != mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eStorageBuffer));
    }

    //check comp.get_active_buffer_ranges(res.push_constant_buffers[0].id);
    for(const auto& res : resources.push_constant_buffers)
    {
        auto it = mPushBlocks.find(res.name);
        if(it != mPushBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mPushBlocks.emplace(res.name, buildPushBlock(&compiler, res, stageFlag));
    }

    for(const auto& res : resources.sampled_images)
    {
        auto it = mUniforms.find(res.name);
        if(it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eCombinedImageSampler));
    }

    for(const auto& res : resources.separate_images)
    {
        auto it = mUniforms.find(res.name);
        if(it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampledImage));
    }

    for(const auto& res : resources.separate_samplers)
    {
        auto it = mUniforms.find(res.name);
        if(it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampler));
    }

    for(const auto& res : resources.storage_images)
    {
        auto it = mUniforms.find(res.name);
        if(it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eStorageImage));
    }

    for(const auto& res : resources.subpass_inputs)
    {
        auto it = mUniforms.find(res.name);
        if(it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eInputAttachment));
    }

    auto constants = compiler.get_specialization_constants();
    for(const auto& constant : constants)
    {
        auto constant_name = compiler.get_name(constant.id);
        auto it = mConstants.find(constant_name);
        if(it != mConstants.end())
            it->second.stageFlags |= stageFlag;
        else
           mConstants.emplace(constant_name, buildConstant(&compiler, constant, stageFlag));
    }

    uint32_t input_offset = 0;
    for(const auto& res : resources.stage_inputs)
        mInputAttributes[stageFlag].emplace(res.name, buildAttribute(&compiler, res, input_offset));

    input_offset = 0;
    for(const auto& res : resources.stage_outputs)
        mOutputAttributes[stageFlag].emplace(res.name, buildAttribute(&compiler, res, input_offset));

    getBindingDescription(vk::ShaderStageFlagBits::eVertex);
}

CUniformBlock CShader::buildUniformBlock(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
    //New uniform block
    const auto& type = compiler->get_type(res.type_id);
    auto& decoration = compiler->get_decoration_bitset(type.self);
    unsigned member_count = type.member_types.size();
    CUniformBlock uniformBlock{};
    uniformBlock.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    uniformBlock.binding = compiler->get_decoration(res.id, spv::DecorationBinding);

    for(uint32_t i = 0; i < member_count; i++)
    {
        size_t array_stride{0}, matrix_stride{0};
        auto &member_type = compiler->get_type(type.member_types[i]);
        CUniform uniform{};
        //compiler.get_member_decoration(, i, spv::DecorationDescriptorSet);
        uniform.size = compiler->get_declared_struct_member_size(type, i);
        uniform.offset = compiler->type_struct_member_offset(type, i);
        if (!member_type.array.empty() && uniform.size == 0) 
            array_stride = compiler->type_struct_member_array_stride(type, i);

        if (member_type.columns > 1) 
            matrix_stride = compiler->type_struct_member_matrix_stride(type, i);

        uniform.stageFlags = stageFlag;
        uniformBlock.mUniforms.emplace(compiler->get_member_name(type.self, i), uniform);
    }

    uniformBlock.descriptorType = descriptorType;
    uniformBlock.size = compiler->get_declared_struct_size(type);
    return uniformBlock;
}

CPushConstBlock CShader::buildPushBlock(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag)
{
    //New push block
    const auto& type = compiler->get_type(res.type_id);
    unsigned member_count = type.member_types.size();
    CPushConstBlock pushBlock{};

    for(uint32_t i = 0; i < member_count; i++)
    {
        auto &member_type = compiler->get_type(type.member_types[i]);
        CUniform uniform{};
        uniform.size = compiler->get_declared_struct_member_size(type, i);
        uniform.offset = compiler->type_struct_member_offset(type, i);
        if (!member_type.array.empty()) { size_t array_stride = compiler->type_struct_member_array_stride(type, i); }
        if (member_type.columns > 1) { size_t matrix_stride = compiler->type_struct_member_matrix_stride(type, i); }
        uniform.stageFlags = stageFlag;
        pushBlock.mUniforms.emplace(compiler->get_member_name(type.self, i), uniform);
    }

    pushBlock.size = compiler->get_declared_struct_size(type);
    return pushBlock;
}

CUniform CShader::buildUnifrom(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
    CUniform uniform{};
    uniform.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    uniform.binding = compiler->get_decoration(res.id, spv::DecorationBinding);
    uniform.descriptorType = descriptorType;
    uniform.stageFlags = stageFlag;
    return uniform;
}

CConstant CShader::buildConstant(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::SpecializationConstant& res, vk::ShaderStageFlagBits stageFlag)
{
    CConstant constant{};
    auto& value = compiler->get_constant(res.id);
    const auto& type = compiler->get_type(value.constant_type);

    constant.constantId = res.constant_id;
    constant.size = parseSize(type);
    constant.offset = 0;
    constant.stageFlags = stageFlag;
    
    return constant;
}

CAttribute CShader::buildAttribute(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::Resource &res, uint32_t& offset)
{
    CAttribute attribute{};
    const auto& type = compiler->get_type(res.type_id);
    attribute.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    attribute.location = compiler->get_decoration(res.id, spv::DecorationLocation);
    attribute.size = parseSize(type);
    attribute.offset = offset;
    offset += attribute.size;
    attribute.type = parseVkFormat(type);
    return attribute;
}

vk::ShaderStageFlagBits CShader::getShaderStage(const std::filesystem::path &moduleName)
{
    auto fileExt = moduleName.extension().string();
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

	if (fileExt == ".comp")
		return vk::ShaderStageFlagBits::eCompute;
	if (fileExt == ".vert")
		return vk::ShaderStageFlagBits::eVertex;
	if (fileExt == ".tesc")
		return vk::ShaderStageFlagBits::eTessellationControl;
	if (fileExt == ".tese")
		return vk::ShaderStageFlagBits::eTessellationEvaluation;
	if (fileExt == ".geom")
		return vk::ShaderStageFlagBits::eGeometry;
	if (fileExt == ".frag")
		return vk::ShaderStageFlagBits::eFragment;
    if (fileExt == ".rchit")
		return vk::ShaderStageFlagBits::eClosestHitKHR;
    if (fileExt == ".rmiss")
		return vk::ShaderStageFlagBits::eMissKHR;
    if (fileExt == ".rgen")
        return vk::ShaderStageFlagBits::eRaygenKHR;
	return vk::ShaderStageFlagBits::eAll;
}

vk::Format CShader::parseVkFormat(const spirv_cross::SPIRType& type)
{
    switch (type.basetype)
    {
        case spirv_cross::SPIRType::SByte:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR8Sint; } break;
                case 2: { return vk::Format::eR8G8Sint; } break;
                case 3: { return vk::Format::eR8G8B8Sint; } break;
                case 4: { return vk::Format::eR8G8B8A8Sint; } break;
            }
        } break;
        case spirv_cross::SPIRType::UByte:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR8Uint; } break;
                case 2: { return vk::Format::eR8G8Uint; } break;
                case 3: { return vk::Format::eR8G8B8Uint; } break;
                case 4: { return vk::Format::eR8G8B8A8Uint; } break;
            }
        } break;
        case spirv_cross::SPIRType::Short:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR16Sint; } break;
                case 2: { return vk::Format::eR16G16Sint; } break;
                case 3: { return vk::Format::eR16G16B16Sint; } break;
                case 4: { return vk::Format::eR16G16B16A16Sint; } break;
            }
        } break;
        case spirv_cross::SPIRType::UShort:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR16Uint; } break;
                case 2: { return vk::Format::eR16G16Uint; } break;
                case 3: { return vk::Format::eR16G16B16Uint; } break;
                case 4: { return vk::Format::eR16G16B16A16Uint; } break;
            }
        } break;
        case spirv_cross::SPIRType::Half:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR16Sfloat; } break;
                case 2: { return vk::Format::eR16G16Sfloat; } break;
                case 3: { return vk::Format::eR16G16B16Sfloat; } break;
                case 4: { return vk::Format::eR16G16B16A16Sfloat; } break;
            }
        } break;
        case spirv_cross::SPIRType::Int:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR32Sint; } break;
                case 2: { return vk::Format::eR32G32Sint; } break;
                case 3: { return vk::Format::eR32G32B32Sint; } break;
                case 4: { return vk::Format::eR32G32B32A32Sint; } break;
            }
        } break;
        case spirv_cross::SPIRType::UInt:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR32Uint; } break;
                case 2: { return vk::Format::eR32G32Uint; } break;
                case 3: { return vk::Format::eR32G32B32Uint; } break;
                case 4: { return vk::Format::eR32G32B32A32Uint; } break;
            }
        } break;
        case spirv_cross::SPIRType::Float:
        {
            switch(type.vecsize)
            {
                case 1: { return vk::Format::eR32Sfloat; } break;
                case 2: { return vk::Format::eR32G32Sfloat; } break;
                case 3: { return vk::Format::eR32G32B32Sfloat; } break;
                case 4: { return vk::Format::eR32G32B32A32Sfloat; } break;
            }
        } break;
    }
    return vk::Format::eUndefined;
}

size_t CShader::parseSize(const spirv_cross::SPIRType& type)
{
    switch (type.basetype)
    {
        case spirv_cross::SPIRType::SByte:
        case spirv_cross::SPIRType::UByte:
        {
            return type.vecsize;
        } break;
        case spirv_cross::SPIRType::Short:
        case spirv_cross::SPIRType::UShort:
        case spirv_cross::SPIRType::Half:
        {
            return 2 * type.vecsize;
        } break;
        case spirv_cross::SPIRType::Int:
        case spirv_cross::SPIRType::UInt:
        case spirv_cross::SPIRType::Float:
        {
            return 4 * type.vecsize;
        } break;
    }
    return std::numeric_limits<size_t>::max();
}