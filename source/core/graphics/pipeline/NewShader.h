#pragma once
#include "graphics/descriptor/DescriptorSet.h"
#include "graphics/descriptor/DescriptorSet.h"

namespace glslang {
class TProgram;
class TType;
}

namespace spirv_cross
{
    class Compiler;
    class Resource;
}

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class CUniform 
            {
            public:
                friend class CShader;
                CUniform() = default;

                uint32_t getSet() const { return set; }
                uint32_t getBinding() const { return binding; }
                uint32_t getOffset() const { return offset; }
                size_t getSize() const { return size; }
                vk::DescriptorType getDescriptorType() const { return descriptorType; }
                vk::ShaderStageFlags getStageFlags() const { return stageFlags; }

            private:
                uint32_t set{0};
                uint32_t binding{0};
                uint32_t offset{0};
                size_t size{0};
                vk::DescriptorType descriptorType;
                vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
            };

            class CUniformBlock 
            {
            public:
                friend class CShader;
                CUniformBlock() = default;

                uint32_t getSet() const { return set; }
                uint32_t getBinding() const { return binding; }
                size_t getSize() const { return size; }
                vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
                vk::DescriptorType getDescriptorType() const { return descriptorType; }
                const std::map<std::string, CUniform> &GetUniforms() const { return mUniforms; }

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
                size_t size{0};
                vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
                vk::DescriptorType descriptorType;
                std::map<std::string, CUniform> mUniforms{};
            };

            class CPushConstBlock 
            {
            public:
                friend class CShader;
                CPushConstBlock() = default;

                size_t getSize() const { return size; }
                vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
                const std::map<std::string, CUniform> &GetUniforms() const { return mUniforms; }

                std::optional<CUniform> getUniform(const std::string &name) const 
                {
                    auto it = mUniforms.find(name);

                    if (it == mUniforms.end())
                        return std::nullopt;

                    return it->second;
                }
            private:
                size_t size{0};
                vk::ShaderStageFlags stageFlags{vk::ShaderStageFlagBits::eAll};
                std::map<std::string, CUniform> mUniforms{};
            };

            class CAttribute 
            {
            public:
                friend class CShader;
                CAttribute() = default;

                uint32_t getSet() const { return set; }
                uint32_t getLocation() const { return location; }
                size_t getSize() const { return size; }
                size_t getOffset() const { return offset; }
                uint32_t getType() const { return type; }

            private:
                uint32_t set{0};
                uint32_t location{0};
                size_t size{0};
                size_t offset{0};
                uint32_t type{0};
            };

            class CConstant 
            {
            public:
                friend class CShader;
                CConstant() = default;

                uint32_t getBinding() const { return binding; }
                size_t getSize() const { return size; }
                vk::ShaderStageFlags getStageFlags() const { return stageFlags; }
                int32_t getGlType() const { return type; }
            private:
                uint32_t binding;
                size_t size;
                vk::ShaderStageFlags stageFlags;
                uint32_t type;
            };

            class CShader
            {
            public:
                using Define = std::pair<std::string, std::string>;
                CShader() = default;
                ~CShader();

                void addStage(const std::filesystem::path &moduleName, const std::vector<char> &moduleCode, const std::string &preamble);
                void clear();
                void finalizeReflection();
                static vk::ShaderStageFlagBits getShaderStage(const std::filesystem::path &moduleName);

                std::optional<uint32_t> getDescriptorLocation(const std::string &name) const;
                std::optional<uint32_t> getDescriptorSize(const std::string &name) const;
                std::optional<CUniform> getUniform(const std::string &name) const;
                std::optional<CUniformBlock> getUniformBlock(const std::string &name) const;
                std::optional<CAttribute> getInputAttribute(const std::string &name) const;
                std::optional<CAttribute> getOutputAttribute(const std::string &name) const;
                std::vector<vk::PushConstantRange> getPushConstantRanges() const;

                std::optional<vk::DescriptorType> getDescriptorType(uint32_t location) const;

                const std::unordered_map<std::string, CUniform>& getUniforms() const { return m_mUniforms; }
                const std::unordered_map<std::string, CUniformBlock>& getUniformBlocks() const { return m_mUniformBlocks; }
                const std::unordered_map<std::string, CPushConstBlock>& getPushBlocks() const { return m_mPushBlocks; }
                const std::unordered_map<std::string, CAttribute>& getInputAttributes() const { return m_mInputAttributes; }
                const std::unordered_map<std::string, CAttribute>& getOutputAttributes() const { return m_mOutputAttributes; }
                const std::unordered_map<std::string, CConstant>& getConstants() const { return m_mConstants; }

                const std::vector<vk::DescriptorSetLayoutBinding> &getDescriptorSetLayouts() const { return m_vDescriptorSetLayouts; }
	            const std::vector<vk::DescriptorPoolSize> &getDescriptorPools() const { return m_vDescriptorPools; }
	            const std::vector<vk::VertexInputAttributeDescription> &getAttributeDescriptions() const { return m_vAttributeDescriptions; }
	            const vk::VertexInputBindingDescription &getBindingDescription() const { return m_bindingDescriptions; }

                const std::vector<vk::PipelineShaderStageCreateInfo>& getStageCreateInfo() const { return m_vShaderModules; }
                std::vector<vk::PushConstantRange> getPushConstantRanges() const;
            private:
                void buildReflection(std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag);
                CUniformBlock buildUniformBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
                CPushConstBlock buildPushBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag);
                CUniform buildUnifrom(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
                CAttribute buildAttribute(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, uint32_t& offset);

                std::unordered_map<std::string, uint32_t> m_mDescriptorLocations;
	            std::unordered_map<std::string, uint32_t> m_mDescriptorSizes;

                std::unordered_map<std::string, CUniform> m_mUniforms;
                std::unordered_map<std::string, CUniformBlock> m_mUniformBlocks;
                std::unordered_map<std::string, CPushConstBlock> m_mPushBlocks;
                std::unordered_map<std::string, CAttribute> m_mInputAttributes;
                std::unordered_map<std::string, CAttribute> m_mOutputAttributes;
                std::unordered_map<std::string, CConstant> m_mConstants;

                std::vector<vk::DescriptorSetLayoutBinding> m_vDescriptorSetLayouts;
                uint32_t lastDescriptorBinding = 0;
                std::vector<vk::DescriptorPoolSize> m_vDescriptorPools;
                std::unordered_map<uint32_t, vk::DescriptorType> m_mDescriptorTypes;
                std::vector<vk::VertexInputAttributeDescription> m_vAttributeDescriptions;
                vk::VertexInputBindingDescription m_bindingDescriptions;

                std::vector<vk::PipelineShaderStageCreateInfo> m_vShaderModules;
                std::vector<vk::ShaderStageFlagBits> m_vShaderStage;
            };
        }
    }
}