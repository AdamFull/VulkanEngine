#include "Shader.hpp"
#include "Core/VulkanHighLevel.h"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
//#include <spirv_cross/spirv_reflect.hpp>
#include "Core/VulkanStaticHelper.h"
#include "filesystem/FilesystemHelper.h"
#include "Core/DataTypes/VulkanVertex.hpp"

using namespace Engine::Core::Pipeline;

class ShaderIncluder : public glslang::TShader::Includer 
{
public:
	IncludeResult *includeLocal(const char *headerName, const char *includerName, size_t inclusionDepth) override 
    {
		auto directory = fs::path(includerName).parent_path();
		auto fileLoaded = Engine::FilesystemHelper::ReadFile((directory / headerName).string());

		if (fileLoaded.empty()) 
        {
			std::cerr << "Shader Include could not be loaded: " << std::quoted(headerName) << std::endl;
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
			std::cerr << "Shader Include could not be loaded: " << std::quoted(headerName) << std::endl;
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

EShLanguage GetEshLanguage(vk::ShaderStageFlagBits stageFlag) 
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

TBuiltInResource GetResources() 
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

Shader::~Shader()
{
    Clear();
}

std::optional<uint32_t> Shader::GetDescriptorLocation(const std::string &name) const 
{
	if (auto it = m_mDescriptorLocations.find(name); it != m_mDescriptorLocations.end())
		return it->second;
	return std::nullopt;
}

std::optional<uint32_t> Shader::GetDescriptorSize(const std::string &name) const 
{
	if (auto it = m_mDescriptorSizes.find(name); it != m_mDescriptorSizes.end())
		return it->second;
	return std::nullopt;
}

std::optional<Uniform> Shader::GetUniform(const std::string &name) const 
{
	if (auto it = m_mUniforms.find(name); it != m_mUniforms.end())
		return it->second;
	return std::nullopt;
}

std::optional<UniformBlock> Shader::GetUniformBlock(const std::string &name) const 
{
	if (auto it = m_mUniformBlocks.find(name); it != m_mUniformBlocks.end())
		return it->second;
	return std::nullopt;
}

std::optional<Attribute> Shader::GetAttribute(const std::string &name) const 
{
	if (auto it = m_mAttributes.find(name); it != m_mAttributes.end())
		return it->second;
	return std::nullopt;
}

std::vector<vk::PushConstantRange> Shader::GetPushConstantRanges() const 
{
	std::vector<vk::PushConstantRange> pushConstantRanges;
	uint32_t currentOffset = 0;

	for (const auto &[uniformBlockName, uniformBlock] : m_mUniformBlocks) 
    {
		if (uniformBlock.GetType() != UniformBlock::EType::ePushConstant)
			continue;

		vk::PushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = uniformBlock.GetStageFlags();
		pushConstantRange.offset = currentOffset;
		pushConstantRange.size = static_cast<uint32_t>(uniformBlock.GetSize());
		pushConstantRanges.emplace_back(pushConstantRange);
		currentOffset += pushConstantRange.size;
	}
	return pushConstantRanges;
}

std::optional<vk::DescriptorType> Shader::GetDescriptorType(uint32_t location) const 
{
	if (auto it = m_mDescriptorTypes.find(location); it != m_mDescriptorTypes.end())
		return it->second;
	return std::nullopt;
}

void Shader::AddStage(const std::filesystem::path &moduleName, const std::vector<char> &moduleCode, const std::string &preamble)
{
    m_vShaderStage.emplace_back(GetShaderStage(moduleName));
    auto language = GetEshLanguage(m_vShaderStage.back());
	glslang::TProgram program;
	glslang::TShader shader(language);
	auto resources = GetResources();

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

    ShaderIncluder includer;

	std::string str;

	if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &str, includer)) 
    {
        std::cerr << "****************SHADER_PREPROCESSING_FAILED****************" << std::endl;
        std::cerr << "Shader: " << moduleName << std::endl;
        std::cerr << shader.getInfoLog() << std::endl;
        std::cerr << shader.getInfoDebugLog() << std::endl;
        std::cerr << "***********************************************************" << std::endl;
        assert(false && "SPRIV shader preprocess failed!");
	}

	if (!shader.parse(&resources, defaultVersion, true, messages, includer)) 
    {
        std::cerr << "*******************SHADER_PARSING_FAILED*******************" << std::endl;
		std::cerr << "Shader: " << moduleName << std::endl;
        std::cerr << shader.getInfoLog() << std::endl;
        std::cerr << shader.getInfoDebugLog() << std::endl;
        std::cerr << "***********************************************************" << std::endl;
        assert(false && "SPRIV shader parse failed!");
	}

    program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO()) 
    {
        std::cerr << "*******************SHADER_LINKING_FAILED*******************" << std::endl;
        std::cerr << "Shader: " << moduleName << std::endl;
        std::cerr << shader.getInfoLog() << std::endl;
        std::cerr << shader.getInfoDebugLog() << std::endl;
        std::cerr << "***********************************************************" << std::endl;
        assert(false && "Error while linking shader program.");
	}

    if(!program.buildReflection())
    {
        std::cerr << "***************SHADER_BUILD_REFLECTION_FAILED**************" << std::endl;
        std::cerr << "Shader: " << moduleName << std::endl;
        std::cerr << shader.getInfoLog() << std::endl;
        std::cerr << shader.getInfoDebugLog() << std::endl;
        std::cerr << "***********************************************************" << std::endl;
        assert(false && "Error while building shader reflection.");
    }

    for (uint32_t dim = 0; dim < 3; ++dim) 
    {
		if (auto localSize = program.getLocalSize(dim); localSize > 1)
			m_localSizes[dim] = localSize;
	}

    for (int32_t i = program.getNumLiveUniformBlocks() - 1; i >= 0; i--)
		LoadUniformBlock(program, m_vShaderStage.back(), i);

	for (int32_t i = 0; i < program.getNumLiveUniformVariables(); i++)
		LoadUniform(program, m_vShaderStage.back(), i);

	for (int32_t i = 0; i < program.getNumPipeInputs(); i++)
		LoadAttribute(program, m_vShaderStage.back(), i);

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
	std::vector<uint32_t> spirv;
	GlslangToSpv(*program.getIntermediate(static_cast<EShLanguage>(language)), spirv, &logger, &spvOptions);

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

void Shader::Clear()
{
    for(auto& stage : m_vShaderModules)
    {
        UDevice->Destroy(stage.module);
    }
    m_vShaderModules.clear();
    m_vShaderStage.clear();
    m_mDescriptorLocations.clear();
    m_mDescriptorSizes.clear();
    m_mUniforms.clear();
    m_mUniformBlocks.clear();
    m_mAttributes.clear();
    m_mConstants.clear();
    //Destroy before clear
    m_vDescriptorSetLayouts.clear();
    m_vDescriptorPools.clear();
    m_mDescriptorTypes.clear();
    m_vAttributeDescriptions.clear();
    lastDescriptorBinding = 0;
}

void Shader::BuildReflection()
{
    std::unordered_map<vk::DescriptorType, uint32_t> descriptorPoolCounts;

    // Process to descriptors.
	for (const auto &[uniformBlockName, uniformBlock] : m_mUniformBlocks) 
    {
		vk::DescriptorType descriptorType{};

        switch (uniformBlock.m_eType) 
        {
		case UniformBlock::EType::eUniform:
			descriptorType = vk::DescriptorType::eUniformBuffer;
			m_vDescriptorSetLayouts.emplace_back(MakeDescriptorSetLayoutBinding(static_cast<uint32_t>(uniformBlock.m_iBinding), descriptorType, uniformBlock.m_stageFlags, 1));
			break;
		case UniformBlock::EType::eStorage:
			descriptorType = vk::DescriptorType::eStorageBuffer;
			m_vDescriptorSetLayouts.emplace_back(MakeDescriptorSetLayoutBinding(static_cast<uint32_t>(uniformBlock.m_iBinding), descriptorType, uniformBlock.m_stageFlags, 1));
			break;
		case UniformBlock::EType::ePushConstant:
			// Push constants are described in the pipeline.
			break;
		default:
			break;
		}

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
		m_mDescriptorLocations.emplace(uniformBlockName, uniformBlock.m_iBinding);
		m_mDescriptorSizes.emplace(uniformBlockName, uniformBlock.m_iSize);
    }

    for (const auto &[uniformName, uniform] : m_mUniforms) 
    {
		vk::DescriptorType descriptorType{};

		switch (uniform.m_iGlType) 
        {
		case 0x8B5E: // GL_SAMPLER_2D
		case 0x904D: // GL_IMAGE_2D
		case 0x8DC1: // GL_TEXTURE_2D_ARRAY
		case 0x9108: // GL_SAMPLER_2D_MULTISAMPLE
		case 0x9055: // GL_IMAGE_2D_MULTISAMPLE
			descriptorType = uniform.m_bWriteOnly ? vk::DescriptorType::eStorageImage : vk::DescriptorType::eCombinedImageSampler;
			m_vDescriptorSetLayouts.emplace_back(MakeDescriptorSetLayoutBinding(static_cast<uint32_t>(uniform.m_iBinding), descriptorType, uniform.m_stageFlags, 1));
			break;
		case 0x8B60: // GL_SAMPLER_CUBE
		case 0x9050: // GL_IMAGE_CUBE
		case 0x9054: // GL_IMAGE_CUBE_MAP_ARRAY
			descriptorType = uniform.m_bWriteOnly ? vk::DescriptorType::eStorageImage : vk::DescriptorType::eCombinedImageSampler;
			m_vDescriptorSetLayouts.emplace_back(MakeDescriptorSetLayoutBinding(static_cast<uint32_t>(uniform.m_iBinding), descriptorType, uniform.m_stageFlags, 1));
			break;
		default:
			break;
		}

		IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
		m_mDescriptorLocations.emplace(uniformName, uniform.m_iBinding);
		m_mDescriptorSizes.emplace(uniformName, uniform.m_iSize);
	}

    for (const auto &[type, descriptorCount] : descriptorPoolCounts) 
    {
		vk::DescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = type;
		descriptorPoolSize.descriptorCount = descriptorCount;
		m_vDescriptorPools.emplace_back(descriptorPoolSize);
	}

    m_vDescriptorPools.resize(6);
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

	//Sorting attributes by location
	/*std::vector<std::pair<std::string, Attribute>> sortingvec;
	std::vector<std::string> vIgnore{"gl_InstanceIndex", "gl_VertexIndex", "gl_PerVertex"};
	std::for_each(m_mAttributes.begin(), m_mAttributes.end(), [&](const auto &pair)
	{
		auto it = std::find(vIgnore.begin(), vIgnore.end(), pair.first);
		if(it == vIgnore.end())
			sortingvec.emplace_back(pair);
	});

	std::sort(sortingvec.begin(), sortingvec.end(), 
    [](const auto &l, const auto &r) 
    {
		return l.second.GetLocation() < r.second.GetLocation();
	});

	m_mAttributes.clear();
	std::for_each(sortingvec.begin(), sortingvec.end(), [&](const auto &pair){ m_mAttributes.emplace(pair); });

	// Process attribute descriptions.
	uint32_t currentOffset{0};
    for (const auto &[attributeName, attribute] : m_mAttributes) 
    {
		vk::VertexInputAttributeDescription attributeDescription = {};
		attributeDescription.location = static_cast<uint32_t>(attribute.m_iLocation);
		attributeDescription.binding = 0;
		attributeDescription.format = VulkanStaticHelper::GLFormatToVkFormat(attribute.m_iGlType);
		attributeDescription.offset = currentOffset;
		m_vAttributeDescriptions.emplace_back(attributeDescription);
		currentOffset += attribute.m_iSize;
	}

    m_bindingDescriptions.binding = 0;
    m_bindingDescriptions.stride = currentOffset;
    m_bindingDescriptions.inputRate = vk::VertexInputRate::eVertex;*/
}

vk::ShaderStageFlagBits Shader::GetShaderStage(const std::filesystem::path &moduleName)
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

vk::DescriptorSetLayoutBinding Shader::MakeDescriptorSetLayoutBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stage, uint32_t count) 
{
	vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = binding;
	descriptorSetLayoutBinding.descriptorType = descriptorType;
	descriptorSetLayoutBinding.descriptorCount = count;
	descriptorSetLayoutBinding.stageFlags = stage;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
	return descriptorSetLayoutBinding;
}

void Shader::IncrementDescriptorPool(std::unordered_map<vk::DescriptorType, uint32_t> &descriptorPoolCounts, vk::DescriptorType type)
{
	if (auto it = descriptorPoolCounts.find(type); it != descriptorPoolCounts.end())
		it->second++;
	else
		descriptorPoolCounts.emplace(type, 1);
}

void Shader::LoadUniformBlock(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i) 
{
	auto reflection = program.getUniformBlock(i);

	for (auto &[uniformBlockName, uniformBlock] : m_mUniformBlocks) 
    {
		if (uniformBlockName == reflection.name) 
        {
			uniformBlock.m_stageFlags |= stageFlag;
			return;
		}
	}

	auto type = UniformBlock::EType::eNone;
	if (reflection.getType()->getQualifier().storage == glslang::EvqUniform)
		type = UniformBlock::EType::eUniform;
	if (reflection.getType()->getQualifier().storage == glslang::EvqBuffer)
		type = UniformBlock::EType::eStorage;
	if (reflection.getType()->getQualifier().layoutPushConstant)
		type = UniformBlock::EType::ePushConstant;
    
    UniformBlock uniformBlock{};
    uniformBlock.SetBinding(reflection.getBinding());
    uniformBlock.SetSize(reflection.size);
    uniformBlock.SetStageFlags(stageFlag);
    uniformBlock.SetType(type);

	m_mUniformBlocks.emplace(reflection.name, std::move(uniformBlock));
}

void Shader::LoadUniform(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i) 
{
	auto reflection = program.getUniform(i);

	if (reflection.getBinding() == -1) 
    {
		auto splitName = StringHelper::Split(reflection.name, '.');

		if (splitName.size() > 1) {
			for (auto &[uniformBlockName, uniformBlock] : m_mUniformBlocks) 
            {
				if (uniformBlockName == splitName.at(0)) 
                {
                    Uniform uniform{};
                    uniform.SetBinding(reflection.getBinding());
                    uniform.SetOffset(reflection.offset);
                    uniform.SetSize(ComputeSize(reflection.getType()));
                    uniform.SetGlType(reflection.glDefineType);
                    uniform.SetStageFlags(stageFlag);
					uniformBlock.m_mUniforms.emplace(StringHelper::ReplaceFirst(reflection.name, splitName.at(0) + ".", ""), std::move(uniform));
					return;
				}
			}
		}
	}

	for (auto &[uniformName, uniform] : m_mUniforms) 
    {
		if (uniformName == reflection.name) 
        {
			uniform.m_stageFlags |= stageFlag;
			return;
		}
	}

	auto &qualifier = reflection.getType()->getQualifier();
    Uniform uniform{};
    uniform.SetBinding(reflection.getBinding());
    uniform.SetOffset(reflection.offset);
    uniform.SetSize(-1);
    uniform.SetGlType(reflection.glDefineType);
    uniform.SetReadOnly(qualifier.readonly);
    uniform.SetWriteOnly(qualifier.writeonly);
    uniform.SetStageFlags(stageFlag);
	m_mUniforms.emplace(reflection.name, std::move(uniform));
}

void Shader::LoadAttribute(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i) 
{
	auto reflection = program.getPipeInput(i);

	if (reflection.name.empty())
		return;

	for (const auto &[attributeName, attribute] : m_mAttributes) 
    {
		if (attributeName == reflection.name)
			return;
	}

	auto &qualifier = reflection.getType()->getQualifier();
    Attribute attribute{};
    attribute.SetSet(qualifier.layoutSet);
    attribute.SetLocation(qualifier.layoutLocation);
    attribute.SetSize(ComputeSize(reflection.getType()));
    attribute.SetGlType(reflection.glDefineType);
	m_mAttributes.emplace(reflection.name, std::move(attribute));
}

int32_t Shader::ComputeSize(const glslang::TType *ttype) 
{
	// TODO: glslang::TType::computeNumComponents is available but has many issues resolved in this method.
	int32_t components = 0;

	if (ttype->getBasicType() == glslang::EbtStruct || ttype->getBasicType() == glslang::EbtBlock) 
    {
		for (const auto &tl : *ttype->getStruct())
			components += ComputeSize(tl.type);
	} 
    else if (ttype->getMatrixCols() != 0) 
    {
		components = ttype->getMatrixCols() * ttype->getMatrixRows();
	} 
    else 
    {
		components = ttype->getVectorSize();
	}

	if (ttype->getArraySizes()) 
    {
		int32_t arraySize = 1;

		for (int32_t d = 0; d < ttype->getArraySizes()->getNumDims(); ++d) 
        {
			// This only makes sense in paths that have a known array size.
			if (auto dimSize = ttype->getArraySizes()->getDimSize(d); dimSize != glslang::UnsizedArraySize)
				arraySize *= dimSize;
		}

		components *= arraySize;
	}

	return sizeof(float) * ttype->computeNumComponents();
}