#pragma once
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include "graphics/descriptor/DescriptorSet.h"
#include "graphics/descriptor/DescriptorSet.h"

namespace glslang
{
	class TProgram;
	class TType;
}

namespace spirv_cross
{
	class CompilerGLSLExt : public CompilerGLSL
	{
	public:
		explicit CompilerGLSLExt(std::vector<uint32_t> spirv_) : CompilerGLSL(std::move(spirv_)) {}
		BufferPackingStandard get_packing_standart(const SPIRType &type);
		uint32_t get_packed_base_size(const SPIRType &type);
		uint32_t get_packed_alignment(const SPIRType &type, const Bitset &flags);
		uint32_t get_packed_array_stride(const SPIRType &type, const Bitset &flags);
		uint32_t get_packed_size(const SPIRType &type, const Bitset &flags);
	};
}

namespace engine
{
	namespace core
	{
		namespace pipeline
		{
			/**
			 * @brief Shader uniform representation class
			 *
			 */
			class CUniform
			{
			public:
				friend class CShader;
				CUniform() = default;

				/**
				 * @brief Get uniform descriptor set number
				 *
				 * @return uint32_t Descriptor set number
				 */
				uint32_t getSet() const { return set; }

				/**
				 * @brief Get uniform descriptor set binding
				 *
				 * @return uint32_t Descriptor binding number
				 */
				uint32_t getBinding() const { return binding; }

				/**
				 * @brief Get uniform offset
				 *
				 * @return uint32_t Uniform offset
				 */
				std::size_t getOffset() const { return offset; }

				/**
				 * @brief Get uniform size
				 *
				 * @return size_t Uniform size
				 */
				std::size_t getSize() const { return size; }

				/**
				 * @brief Get uniform descriptor type
				 *
				 * @return vk::DescriptorType Uniform descriptor type
				 */
				vk::DescriptorType getDescriptorType() const { return descriptorType; }

				/**
				 * @brief Get unifrom stage flags
				 *
				 * @return vk::ShaderStageFlags Unifrom stage flags
				 */
				vk::ShaderStageFlags getStageFlags() const { return stageFlags; }

			private:
				uint32_t set{0};
				uint32_t binding{0};
				std::size_t offset{0};
				std::size_t size{0};
				vk::DescriptorType descriptorType;
				vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
			};

			/**
			 * @brief Shader uniform buffer representation class
			 *
			 */
			class CUniformBlock
			{
			public:
				friend class CShader;
				CUniformBlock() = default;

				/**
				 * @brief Get uniform buffer descriptor set
				 *
				 * @return uint32_t Descriptor set number
				 */
				uint32_t getSet() const { return set; }

				/**
				 * @brief Get uniform buffer descriptor set binding
				 *
				 * @return uint32_t Descriptor set binding
				 */
				uint32_t getBinding() const { return binding; }

				/**
				 * @brief Get uniform buffer size
				 *
				 * @return size_t Uniform buffer size
				 */
				std::size_t getSize() const { return size; }

				/**
				 * @brief Get uniform buffer stage flags
				 *
				 * @return vk::ShaderStageFlags Uniform buffer stage flags
				 */
				vk::ShaderStageFlags getStageFlags() const { return stageFlags; }

				/**
				 * @brief Get uniform buffer descriptor type
				 *
				 * @return vk::DescriptorType Descriptor type
				 */
				vk::DescriptorType getDescriptorType() const { return descriptorType; }

				/**
				 * @brief Get unifrom buffer uniforms
				 *
				 * @return const std::map<std::string, CUniform>& Uniform buffer included uniforms
				 */
				const std::map<std::string, CUniform> &GetUniforms() const { return mUniforms; }

				/**
				 * @brief Get concrete uniform
				 *
				 * @param name Uniform name
				 * @return std::optional<CUniform> Uniform optional object
				 */
				std::optional<CUniform> getUniform(const std::string &name) const
				{
					auto it = mUniforms.find(name);

					if (it == mUniforms.end())
						return std::nullopt;

					return it->second;
				}

			private:
				uint32_t set{0};
				uint32_t binding{0};
				std::size_t size{0};
				vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
				vk::DescriptorType descriptorType;
				std::map<std::string, CUniform> mUniforms{};
			};

			/**
			 * @brief Push constant block representation class
			 *
			 */
			class CPushConstBlock
			{
			public:
				friend class CShader;
				CPushConstBlock() = default;

				/**
				 * @brief Get push block size
				 *
				 * @return size_t Bloack size
				 */
				std::size_t getSize() const { return size; }

				/**
				 * @brief Get push block stages
				 *
				 * @return vk::ShaderStageFlags Push block shader stages
				 */
				vk::ShaderStageFlags getStageFlags() const { return stageFlags; }

				/**
				 * @brief Get push constant block
				 *
				 * @return const std::map<std::string, CUniform>& Push block included uniforms
				 */
				const std::map<std::string, CUniform> &GetUniforms() const { return mUniforms; }

				/**
				 * @brief Get concrete uniform
				 *
				 * @param name Uniform name
				 * @return std::optional<CUniform> Uniform optional object
				 */
				std::optional<CUniform> getUniform(const std::string &name) const
				{
					auto it = mUniforms.find(name);

					if (it == mUniforms.end())
						return std::nullopt;

					return it->second;
				}

			private:
				std::size_t size{0};
				vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
				std::map<std::string, CUniform> mUniforms{};
			};

			/**
			 * @brief Shader attribute representation class
			 *
			 */
			class CAttribute
			{
			public:
				friend class CShader;
				CAttribute() = default;

				/**
				 * @brief Get attribute set
				 *
				 * @return uint32_t Attribute set
				 */
				uint32_t getSet() const { return set; }

				/**
				 * @brief Get attribute location
				 *
				 * @return uint32_t Attribute location
				 */
				uint32_t getLocation() const { return location; }

				/**
				 * @brief Get attribute size
				 *
				 * @return size_t Attribute size
				 */
				std::size_t getSize() const { return size; }

				/**
				 * @brief Get attribute offset
				 *
				 * @return size_t Attribute offset
				 */
				std::size_t getOffset() const { return offset; }

				/**
				 * @brief Get Attribute vulkan format
				 *
				 * @return vk::Format Attribute vulkan format
				 */
				vk::Format getType() const { return type; }

			private:
				uint32_t set{0};
				uint32_t location{0};
				std::size_t size{0};
				std::size_t offset{0};
				vk::Format type{};
			};

			/**
			 * @brief Stader constant representation class
			 *
			 */
			class CConstant
			{
			public:
				friend class CShader;
				CConstant() = default;

				/**
				 * @brief Get constant binding
				 *
				 * @return uint32_t Constant binding
				 */
				uint32_t getConstantId() const { return constantId; }

				/**
				 * @brief Get constant size
				 *
				 * @return size_t Constant size
				 */
				size_t getSize() const { return size; }

				/**
				 * @brief Get constant offset
				 * 
				 * @return size_t 
				 */
				size_t getOffset() const { return offset; }

				/**
				 * @brief Get push block stages
				 *
				 * @return vk::ShaderStageFlags Push block shader stages
				 */
				vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
			private:
				uint32_t constantId{0};
				size_t size{0};
				size_t offset{0};
				vk::ShaderStageFlags stageFlags{};
			};

			/**
			 * @brief Vulkan shader with reflection based on glslang and spirv-cross together.
			 * Shader representation class.
			 *
			 */
			class CShader
			{
			public:
				using Define = std::pair<std::string, std::string>;
				CShader() = default;
				~CShader();

				/**
				 * @brief Add shader stage
				 *
				 * @param moduleName GLSL module file path
				 * @param moduleCode GLSL module code
				 * @param preamble Definition blocks
				 */
				void addStage(const std::filesystem::path &moduleName, const std::string &moduleCode, const std::string &preamble);

				/**
				 * @brief Set times of using descriptors for shader. Because some shaders can be multiple instanced and render can have multiple frames in flight.
				 * 
				 * @param multiplier number of multiplications
				 */
				void setDescriptorMultiplier(uint32_t multiplier) { descriptorMultiplier = multiplier; }

				/**
				 * @brief Clear shader reflection info
				 *
				 */
				void clear();

				/**
				 * @brief Finish building shader reflection
				 *
				 */
				void finalizeReflection();

				std::optional<CUniform> getUniform(const std::string &name) const;
				std::optional<CUniformBlock> getUniformBlock(const std::string &name) const;
				std::optional<CPushConstBlock> getPushBlock(const std::string &name) const;
				std::optional<CConstant> getConstant(const std::string &name) const;
				std::optional<CAttribute> getInputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const;
				std::optional<CAttribute> getOutputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const;
				std::vector<vk::PushConstantRange> getPushConstantRanges() const;

				/**
				 * @brief Get shader local sizes
				 *
				 * @return const std::array<std::optional<uint32_t>, 3>& Array with shader local sizes
				 */
				const std::array<std::optional<uint32_t>, 3> &getLocalSizes() const { return localSizes; }

				/**
				 * @brief Get uniforms from shader
				 *
				 * @return const std::unordered_map<std::string, CUniform>& Unifroms map
				 */
				const std::unordered_map<std::string, CUniform> &getUniforms() const { return mUniforms; }

				/**
				 * @brief Get shader uniform blocks
				 *
				 * @return const std::unordered_map<std::string, CUniformBlock>& Uniform blocks map
				 */
				const std::unordered_map<std::string, CUniformBlock> &getUniformBlocks() const { return mUniformBlocks; }

				/**
				 * @brief Get push blocks
				 *
				 * @return const std::unordered_map<std::string, CPushConstBlock>& Push blocks map
				 */
				const std::unordered_map<std::string, CPushConstBlock> &getPushBlocks() const { return mPushBlocks; }

				/**
				 * @brief Get input attributes
				 *
				 * @param stage Shader stage
				 * @return const std::optional<std::unordered_map<std::string, CAttribute>> Optional value with map that contain names and them's attributes
				 */
				const std::optional<std::unordered_map<std::string, CAttribute>> getInputAttributes(vk::ShaderStageFlagBits stage) const;

				/**
				 * @brief Get output attributes
				 *
				 * @param stage Shader stage
				 * @return const std::optional<std::unordered_map<std::string, CAttribute>> Optional value with map that contain names and them's attributes
				 */
				const std::optional<std::unordered_map<std::string, CAttribute>> getOutputAttributes(vk::ShaderStageFlagBits stage) const;

				/**
				 * @brief Get constants
				 *
				 * @return const std::unordered_map<std::string, CConstant>& Map with associated names and constants
				 */
				const std::unordered_map<std::string, CConstant> &getConstants() const { return mConstants; }

				/**
				 * @brief Get descriptor set layout bindings
				 *
				 * @return const std::vector<vk::DescriptorSetLayoutBinding>& Descriptor set layout bindings vector
				 */
				const std::vector<vk::DescriptorSetLayoutBinding> &getDescriptorSetLayouts() const { return vDescriptorSetLayouts; }

				/**
				 * @brief Get descriptor pools
				 *
				 * @return const std::vector<vk::DescriptorPoolSize>& Vector with descriptor pool sizes
				 */
				const std::vector<vk::DescriptorPoolSize> &getDescriptorPools() const { return vDescriptorPools; }

				/**
				 * @brief Get attributes description by stage
				 *
				 * @param stage Shader stage
				 * @return const std::vector<vk::VertexInputAttributeDescription> Vector of shader input attributes description
				 */
				const std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions(vk::ShaderStageFlagBits stage) const;

				/**
				 * @brief Get binding description
				 *
				 * @param stage Shader stage
				 * @return const vk::VertexInputBindingDescription Vertex input binding description
				 */
				const vk::VertexInputBindingDescription getBindingDescription(vk::ShaderStageFlagBits stage) const;

				/**
				 * @brief Get pipeline shader stage create info
				 *
				 * @return const std::vector<vk::PipelineShaderStageCreateInfo>& vector of shader stages
				 */
				const std::vector<vk::PipelineShaderStageCreateInfo> &getStageCreateInfo() const { return vShaderModules; }

			private:

				void buildSpecializationInfo();

				void createShaderModule(const std::vector<uint32_t>& spirv);

				/**
				 * @brief Build reflection for shader stage
				 *
				 * @param spirv Shader SPIR-V code
				 * @param stageFlag Sg=hader stage flag
				 */
				void buildReflection(std::vector<uint32_t> &spirv, vk::ShaderStageFlagBits stageFlag);

				/**
				 * @brief Helper for parsing uniform block
				 *
				 * @param compiler Spirv cross compiler object
				 * @param res Spirv cross resource object
				 * @param stageFlag Shader stage flag
				 * @param descriptorType Descriptor type
				 * @return CUniformBlock Builded uniform block with included uniforms
				 */
				CUniformBlock buildUniformBlock(spirv_cross::CompilerGLSLExt *compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);

				/**
				 * @brief Helper for parsing push constant block
				 *
				 * @param compiler Spirv cross compiler object
				 * @param res Spirv cross resource object
				 * @param stageFlag Shader stage flag
				 * @return CPushConstBlock Builded push constant block with included uniforms
				 */
				CPushConstBlock buildPushBlock(spirv_cross::CompilerGLSLExt *compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag);

				/**
				 * @brief Helper for parsing single uniforms (texture bindings for example)
				 *
				 * @param compiler Spirv cross compiler object
				 * @param res Spirv cross resource object
				 * @param stageFlag Shader stage flag
				 * @param descriptorType Descriptor type
				 * @return CUniform Uniform with all built in parameters
				 */
				CUniform buildUnifrom(spirv_cross::CompilerGLSLExt *compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);

				/**
				 * @brief Helper for shader specialization constants parsing
				 * 
				 * @param compiler Spirv cross compiler object
				 * @param res Spirv cross constant object
				 * @param stageFlag Shader stage flag
				 * @return CConstant 
				 */
				CConstant buildConstant(spirv_cross::CompilerGLSLExt* compiler, const spirv_cross::SpecializationConstant& res, vk::ShaderStageFlagBits stageFlag);

				/**
				 * @brief Helper for shader  attributes parsing
				 *
				 * @param compiler Spirv cross compiler object
				 * @param res Spirv cross resource object
				 * @param stageFlag Shader stage flag
				 * @return CAttribute
				 */
				CAttribute buildAttribute(spirv_cross::CompilerGLSLExt *compiler, const spirv_cross::Resource &res, uint32_t &offset);

				/**
				 * @brief Get helper for automatically detect shader stage
				 *
				 * @param moduleName Shader module path
				 * @return vk::ShaderStageFlagBits Shader stage flag
				 */
				static vk::ShaderStageFlagBits getShaderStage(const std::filesystem::path &moduleName);

				/**
				 * @brief Helper for parse vk::Format from spirv-cross type
				 *
				 * @param type SPIR-V cross type object
				 * @return vk::Format Vulkan format
				 */
				static vk::Format parseVkFormat(const spirv_cross::SPIRType &type);

				/**
				 * @brief Helper for calculating size from spirv-cross type
				 *
				 * @param type SPIR-V cross type object
				 * @return size_t Object real size
				 */
				static size_t parseSize(const spirv_cross::SPIRType &type);

				uint32_t descriptorMultiplier{1};

				std::array<std::optional<uint32_t>, 3> localSizes;
				uint32_t executionModeInvocations{0};
				uint32_t executionModeOutputVertices{0};

				std::unordered_map<std::string, CUniform> mUniforms;
				std::unordered_map<std::string, CUniformBlock> mUniformBlocks;
				std::unordered_map<std::string, CPushConstBlock> mPushBlocks;
				std::unordered_map<std::string, CConstant> mConstants;
				std::map<vk::ShaderStageFlagBits, std::unordered_map<std::string, CAttribute>> mInputAttributes;
				std::map<vk::ShaderStageFlagBits, std::unordered_map<std::string, CAttribute>> mOutputAttributes;

				std::vector<vk::DescriptorSetLayoutBinding> vDescriptorSetLayouts;
				uint32_t lastDescriptorBinding = 0;
				std::vector<vk::DescriptorPoolSize> vDescriptorPools;

				std::map<vk::ShaderStageFlagBits, std::vector<vk::SpecializationMapEntry>> mMapEntries;
				std::map<vk::ShaderStageFlagBits, vk::SpecializationInfo> mSpecializationInfo{};
				std::vector<vk::PipelineShaderStageCreateInfo> vShaderModules;
				std::vector<vk::ShaderStageFlagBits> vShaderStage;
			};
		}
	}
}