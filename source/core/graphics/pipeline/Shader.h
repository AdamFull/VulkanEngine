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
    struct SPIRType;
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
                vk::Format getType() const { return type; }

            private:
                uint32_t set{0};
                uint32_t location{0};
                size_t size{0};
                size_t offset{0};
                vk::Format type{};
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

            //remove descriptors buffer maybe
            class CShader
            {
            public:
                using Define = std::pair<std::string, std::string>;
                CShader() = default;
                ~CShader();

                void addStage(const std::filesystem::path &moduleName, const std::string& moduleCode, const std::string &preamble);
                void clear();
                void finalizeReflection();

                std::optional<uint32_t> getDescriptorLocation(const std::string &name) const;
                std::optional<uint64_t> getDescriptorSize(const std::string &name) const;
                std::optional<CUniform> getUniform(const std::string &name) const;
                std::optional<CUniformBlock> getUniformBlock(const std::string &name) const;
                std::optional<CPushConstBlock> getPushBlock(const std::string &name) const;
                std::optional<CAttribute> getInputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const;
                std::optional<CAttribute> getOutputAttribute(const std::string &name, vk::ShaderStageFlagBits stage) const;
                std::vector<vk::PushConstantRange> getPushConstantRanges() const;

                const std::array<std::optional<uint32_t>, 3>& getLocalSizes() const { return localSizes; }
                const std::unordered_map<std::string, CUniform>& getUniforms() const { return mUniforms; }
                const std::unordered_map<std::string, CUniformBlock>& getUniformBlocks() const { return mUniformBlocks; }
                const std::unordered_map<std::string, CPushConstBlock>& getPushBlocks() const { return mPushBlocks; }
                const std::optional<std::unordered_map<std::string, CAttribute>> getInputAttributes(vk::ShaderStageFlagBits stage) const;
                const std::optional<std::unordered_map<std::string, CAttribute>> getOutputAttributes(vk::ShaderStageFlagBits stage) const;
                const std::unordered_map<std::string, CConstant>& getConstants() const { return mConstants; }

                const std::vector<vk::DescriptorSetLayoutBinding> &getDescriptorSetLayouts() const { return vDescriptorSetLayouts; }
	            const std::vector<vk::DescriptorPoolSize> &getDescriptorPools() const { return vDescriptorPools; }
	            const std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions(vk::ShaderStageFlagBits stage) const;
	            const vk::VertexInputBindingDescription getBindingDescription(vk::ShaderStageFlagBits stage) const;

                const std::vector<vk::PipelineShaderStageCreateInfo>& getStageCreateInfo() const { return vShaderModules; }
            private:
                void buildReflection(std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag);
                CUniformBlock buildUniformBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
                CPushConstBlock buildPushBlock(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag);
                CUniform buildUnifrom(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType);
                CAttribute buildAttribute(spirv_cross::Compiler* compiler, const spirv_cross::Resource &res, uint32_t& offset);

                static vk::ShaderStageFlagBits getShaderStage(const std::filesystem::path &moduleName);
                static vk::Format parseVkFormat(const spirv_cross::SPIRType& type);
                static size_t parseSize(const spirv_cross::SPIRType& type);

                std::array<std::optional<uint32_t>, 3> localSizes;

                std::unordered_map<std::string, uint32_t> mDescriptorLocations;
	            std::unordered_map<std::string, uint64_t> mDescriptorSizes;

                std::unordered_map<std::string, CUniform> mUniforms;
                std::unordered_map<std::string, CUniformBlock> mUniformBlocks;
                std::unordered_map<std::string, CPushConstBlock> mPushBlocks;
                std::map<vk::ShaderStageFlagBits, std::unordered_map<std::string, CAttribute>> mInputAttributes;
                std::map<vk::ShaderStageFlagBits, std::unordered_map<std::string, CAttribute>> mOutputAttributes;
                std::unordered_map<std::string, CConstant> mConstants;

                std::vector<vk::DescriptorSetLayoutBinding> vDescriptorSetLayouts;
                uint32_t lastDescriptorBinding = 0;
                std::vector<vk::DescriptorPoolSize> vDescriptorPools;

                std::vector<vk::PipelineShaderStageCreateInfo> vShaderModules;
                std::vector<vk::ShaderStageFlagBits> vShaderStage;
            };
        }
    }
}