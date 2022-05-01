#include "NewShader.h"
#include "graphics/VulkanHighLevel.h"
#include "ShaderCache.h"
#include "filesystem/FilesystemHelper.h"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <spirv_cross.hpp>

//Spirv cross reflection doc
//https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide

using namespace Engine::Core::Pipeline;

class CShaderIncluder : public glslang::TShader::Includer 
{
public:
	IncludeResult *includeLocal(const char *headerName, const char *includerName, size_t inclusionDepth) override 
    {
		auto directory = fs::path(includerName).parent_path();
		auto fileLoaded = Engine::FilesystemHelper::ReadFile((directory / headerName).string());

		if (fileLoaded.empty()) 
        {
			std::stringstream ss;
			ss << "Shader Include could not be loaded: " << std::quoted(headerName);
			utl::logger::log(utl::ELogLevel::eError, ss.str());
			return nullptr;
		}

		auto content = new char[fileLoaded.size()];
		std::memcpy(content, fileLoaded.data(), fileLoaded.size());
		return new IncludeResult(headerName, content, fileLoaded.size(), content);
	}

	IncludeResult *includeSystem(const char *headerName, const char *includerName, size_t inclusionDepth) override 
    {
		auto fileLoaded = Engine::FilesystemHelper::ReadFile(headerName);

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
    clear();
}

void CShader::addStage(const std::filesystem::path &moduleName, const std::vector<char> &moduleCode, const std::string &preamble)
{
    std::vector<uint32_t> spirv;
    auto spirv_cache = CShaderCache::getInstance()->get(moduleName.filename().string());
    if (!spirv_cache)
    {
        m_vShaderStage.emplace_back(getShaderStage(moduleName));
        auto language = getEshLanguage(m_vShaderStage.back());
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
        shader.setEnvTarget(glslang::EShTargetSpv, UDevice->GetVulkanVersion() >= VK_API_VERSION_1_1 ? glslang::EShTargetSpv_1_3 : glslang::EShTargetSpv_1_0);

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
        CShaderCache::getInstance()->add(moduleName.filename().string(), getShaderStage(moduleName), spirv);
    }
    else
    {
        m_vShaderStage.emplace_back(spirv_cache.value().shaderStage);
        spirv = spirv_cache.value().shaderCode;
    }

    buildReflection(spirv, m_vShaderStage.back());

    try
    {
        auto shaderModule = UDevice->Make<vk::ShaderModule, vk::ShaderModuleCreateInfo>
        (
            vk::ShaderModuleCreateInfo
            {
                vk::ShaderModuleCreateFlags(),
                spirv.size() * sizeof(uint32_t),
                spirv.data()
            }
        );

        m_vShaderModules.emplace_back
        (
            vk::PipelineShaderStageCreateInfo
            {
                vk::PipelineShaderStageCreateFlags(),
                m_vShaderStage.back(),
                shaderModule,
                "main"
            }
        );
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create shader module!");
    }
}

void CShader::clear()
{
    for(auto& stage : m_vShaderModules)
        UDevice->Destroy(stage.module);

    m_vShaderModules.clear();
    m_vShaderStage.clear();
    m_mDescriptorLocations.clear();
    m_mDescriptorSizes.clear();
    m_mUniforms.clear();
    m_mUniformBlocks.clear();
    m_mInputAttributes.clear();
    m_mOutputAttributes.clear();
    m_mConstants.clear();
    //Destroy before clear
    m_vDescriptorSetLayouts.clear();
    m_vDescriptorPools.clear();
    m_mDescriptorTypes.clear();
    m_vAttributeDescriptions.clear();
    lastDescriptorBinding = 0;
}

void CShader::finalizeReflection()
{
    //Preparing descriptors for uniform/storage buffers
    for (const auto &[uniformBlockName, uniformBlock] : m_mUniformBlocks) 
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniformBlock.binding;
        descriptorSetLayoutBinding.descriptorType = uniformBlock.descriptorType;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniformBlock.stageFlags;
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        m_vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);

        m_mDescriptorLocations.emplace(uniformBlockName, uniformBlock.binding);
		m_mDescriptorSizes.emplace(uniformBlockName, uniformBlock.size);
    }

    for (const auto &[uniformName, uniform] : m_mUniforms) 
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniform.binding;
        descriptorSetLayoutBinding.descriptorType = uniform.descriptorType;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniform.stageFlags;
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        m_vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);

        m_mDescriptorLocations.emplace(uniformName, uniform.binding);
		m_mDescriptorSizes.emplace(uniformName, uniform.size);
    }

    m_vDescriptorPools.resize(9);
	m_vDescriptorPools[0].type = vk::DescriptorType::eCombinedImageSampler;
	m_vDescriptorPools[0].descriptorCount = 4096;
	m_vDescriptorPools[1].type = vk::DescriptorType::eUniformBuffer;
	m_vDescriptorPools[1].descriptorCount = 2048;
	m_vDescriptorPools[2].type = vk::DescriptorType::eStorageImage;
	m_vDescriptorPools[2].descriptorCount = 2048;
    m_vDescriptorPools[3].type = vk::DescriptorType::eUniformTexelBuffer;
	m_vDescriptorPools[3].descriptorCount = 2048;
    m_vDescriptorPools[4].type = vk::DescriptorType::eStorageTexelBuffer;
	m_vDescriptorPools[4].descriptorCount = 2048;
    m_vDescriptorPools[5].type = vk::DescriptorType::eStorageBuffer;
	m_vDescriptorPools[5].descriptorCount = 2048;
	m_vDescriptorPools[6].type = vk::DescriptorType::eSampledImage;
	m_vDescriptorPools[6].descriptorCount = 2048;
    m_vDescriptorPools[7].type = vk::DescriptorType::eSampler;
	m_vDescriptorPools[7].descriptorCount = 2048;
    m_vDescriptorPools[8].type = vk::DescriptorType::eInputAttachment;
	m_vDescriptorPools[8].descriptorCount = 2048;

    // Sort descriptors by binding.
	std::sort(m_vDescriptorSetLayouts.begin(), m_vDescriptorSetLayouts.end(), 
    [](const vk::DescriptorSetLayoutBinding &l, const vk::DescriptorSetLayoutBinding &r) 
    {
		return l.binding < r.binding;
	});

    // Gets the last descriptors binding.
	if (!m_vDescriptorSetLayouts.empty())
		lastDescriptorBinding = m_vDescriptorSetLayouts.back().binding;

    // Gets the descriptor type for each descriptor.
	for (const auto &descriptor : m_vDescriptorSetLayouts)
		m_mDescriptorTypes.emplace(descriptor.binding, descriptor.descriptorType);
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
	return vk::ShaderStageFlagBits::eAll;
}

std::optional<uint32_t> CShader::getDescriptorLocation(const std::string &name) const
{
    if (auto it = m_mDescriptorLocations.find(name); it != m_mDescriptorLocations.end())
		return it->second;
	return std::nullopt;
}

std::optional<uint32_t> CShader::getDescriptorSize(const std::string &name) const
{
    if (auto it = m_mDescriptorSizes.find(name); it != m_mDescriptorSizes.end())
		return it->second;
	return std::nullopt;
}

std::optional<CUniform> CShader::getUniform(const std::string &name) const
{
    if (auto it = m_mUniforms.find(name); it != m_mUniforms.end())
		return it->second;
	return std::nullopt;
}

std::optional<CUniformBlock> CShader::getUniformBlock(const std::string &name) const
{
    if (auto it = m_mUniformBlocks.find(name); it != m_mUniformBlocks.end())
		return it->second;
	return std::nullopt;
}

std::optional<CAttribute> CShader::getInputAttribute(const std::string &name) const
{
    if (auto it = m_mInputAttributes.find(name); it != m_mInputAttributes.end())
		return it->second;
	return std::nullopt;
}

std::optional<CAttribute> CShader::getOutputAttribute(const std::string &name) const
{
    if (auto it = m_mOutputAttributes.find(name); it != m_mOutputAttributes.end())
		return it->second;
	return std::nullopt;
}

std::vector<vk::PushConstantRange> CShader::getPushConstantRanges() const
{
    std::vector<vk::PushConstantRange> pushConstantRanges;
	uint32_t currentOffset = 0;

	for (const auto &[pushBlockName, pushBlock] : m_mPushBlocks) 
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

std::optional<vk::DescriptorType> CShader::getDescriptorType(uint32_t location) const
{
    if (auto it = m_mDescriptorTypes.find(location); it != m_mDescriptorTypes.end())
		return it->second;
	return std::nullopt;
}

void CShader::buildReflection(std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag)
{
    spirv_cross::Compiler compiler(spirv);
    auto active = compiler.get_active_interface_variables();
    auto resources = compiler.get_shader_resources(active);
    compiler.set_enabled_interface_variables(move(active));

    //Parsing uniform buffers
    for(const auto& res : resources.uniform_buffers)
    {
        //Looking for the block if already exists and put stage flag
        auto it = m_mUniformBlocks.find(res.name);
        if (it != m_mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eUniformBuffer));
    }

    for(const auto& res : resources.storage_buffers)
    {
        auto it = m_mUniformBlocks.find(res.name);
        if(it != m_mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eStorageBuffer));
    }

    //check comp.get_active_buffer_ranges(res.push_constant_buffers[0].id);
    for(const auto& res : resources.push_constant_buffers)
    {
        auto it = m_mPushBlocks.find(res.name);
        if(it != m_mPushBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mPushBlocks.emplace(res.name, buildPushBlock(&compiler, res, stageFlag));
    }

    for(const auto& res : resources.sampled_images)
    {
        auto it = m_mUniforms.find(res.name);
        if(it != m_mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eCombinedImageSampler));
    }

    for(const auto& res : resources.separate_images)
    {
        auto it = m_mUniforms.find(res.name);
        if(it != m_mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampledImage));
    }

    for(const auto& res : resources.separate_samplers)
    {
        auto it = m_mUniforms.find(res.name);
        if(it != m_mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampler));
    }

    for(const auto& res : resources.storage_images)
    {
        auto it = m_mUniforms.find(res.name);
        if(it != m_mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eStorageImage));
    }

    for(const auto& res : resources.subpass_inputs)
    {
        auto it = m_mUniforms.find(res.name);
        if(it != m_mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            m_mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eInputAttachment));
    }

    uint32_t input_offset = 0;
    for(const auto& res : resources.stage_inputs)
    {
        m_mInputAttributes.emplace(res.name, buildAttribute(&compiler, res, input_offset));
    }

    input_offset = 0;
    for(const auto& res : resources.stage_outputs)
    {
        m_mOutputAttributes.emplace(res.name, buildAttribute(&compiler, res, input_offset));
    }
}

CUniformBlock CShader::buildUniformBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
    //New uniform block
    const auto& type = compiler->get_type(res.type_id);
    unsigned member_count = type.member_types.size();
    CUniformBlock uniformBlock{};
    uniformBlock.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    uniformBlock.binding = compiler->get_decoration(res.id, spv::DecorationBinding);

    for(uint32_t i = 0; i < member_count; i++)
    {
        auto &member_type = compiler->get_type(type.member_types[i]);
        CUniform uniform{};
        //compiler.get_member_decoration(, i, spv::DecorationDescriptorSet);
        uniform.size = compiler->get_declared_struct_member_size(type, i);
        uniform.offset = compiler->type_struct_member_offset(type, i);
        if (!member_type.array.empty()) { size_t array_stride = compiler->type_struct_member_array_stride(type, i); }
        if (member_type.columns > 1) { size_t matrix_stride = compiler->type_struct_member_matrix_stride(type, i); }
        uniform.stageFlags = stageFlag;
        uniformBlock.mUniforms.emplace(compiler->get_member_name(type.self, i), uniform);
    }

    uniformBlock.descriptorType = descriptorType;
    uniformBlock.size = compiler->get_declared_struct_size(type);
    return uniformBlock;
}

CPushConstBlock CShader::buildPushBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag)
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

CUniform CShader::buildUnifrom(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
    CUniform uniform{};
    uniform.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    uniform.binding = compiler->get_decoration(res.id, spv::DecorationBinding);
    uniform.descriptorType = descriptorType;
    uniform.stageFlags = stageFlag;
    return uniform;
}

CAttribute CShader::buildAttribute(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, uint32_t& offset)
{
    CAttribute attribute{};
    const auto& type = compiler->get_type(res.type_id);
    attribute.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
    attribute.location = compiler->get_decoration(res.id, spv::DecorationLocation);
    attribute.size = (type.width/8) * type.vecsize;
    attribute.offset = offset;
    offset += attribute.size;
    attribute.type = static_cast<uint32_t>(type.basetype);

    return attribute;
}