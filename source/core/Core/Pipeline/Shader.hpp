#pragma once
#include "Core/Descriptor/DescriptorSet.h"
#include "Core/Descriptor/DescriptorSet.h"

namespace glslang {
class TProgram;
class TType;
}

namespace Engine
{
    namespace Core
    {
        namespace Pipeline
        {
            class Uniform 
            {
            public:
                friend class Shader;
                Uniform() = default;

                void SetBinding(int32_t binding) { m_iBinding = binding; }
                void SetOffset(int32_t offset) { m_iOffset = offset; }
                void SetSize(int32_t size) { m_iSize = size; }
                void SetGlType(int32_t glType) { m_iGlType = glType; }
                void SetReadOnly(bool readOnly) { m_bReadOnly = readOnly; }
                void SetWriteOnly(bool writeOnly) { m_bWriteOnly = writeOnly; }
                void SetStageFlags(vk::ShaderStageFlagBits stageFlags) { m_stageFlags = stageFlags; }

                int32_t GetBinding() const { return m_iBinding; }
                int32_t GetOffset() const { return m_iOffset; }
                int32_t GetSize() const { return m_iSize; }
                int32_t GetGlType() const { return m_iGlType; }
                bool IsReadOnly() const { return m_bReadOnly; }
                bool IsWriteOnly() const { return m_bWriteOnly; }
                vk::ShaderStageFlags GetStageFlags() const { return m_stageFlags; }

            private:
                int32_t m_iBinding{-1};
                int32_t m_iOffset{-1};
                int32_t m_iSize{-1};
                int32_t m_iGlType{-1};
                bool m_bReadOnly{false};
                bool m_bWriteOnly{false};
                vk::ShaderStageFlags m_stageFlags{vk::ShaderStageFlagBits::eAll};
            };

            class UniformBlock 
            {
            public:
                friend class Shader;
                enum class EType 
                { 
                    eNone, 
                    eUniform, 
                    eStorage, 
                    ePushConstant 
                };

                UniformBlock() = default;

                void SetBinding(int32_t binding) { m_iBinding = binding; }
                void SetSize(int32_t size) { m_iSize = size; }
                void SetStageFlags(vk::ShaderStageFlagBits stageFlags) { m_stageFlags = stageFlags; }
                void SetType(EType type) { m_eType = type; }

                int32_t GetBinding() const { return m_iBinding; }
                int32_t GetSize() const { return m_iSize; }
                vk::ShaderStageFlags GetStageFlags() const { return m_stageFlags; }
                EType GetType() const { return m_eType; }
                const std::map<std::string, Uniform> &GetUniforms() const { return m_mUniforms; }

                std::optional<Uniform> GetUniform(const std::string &name) const 
                {
                    auto it = m_mUniforms.find(name);

                    if (it == m_mUniforms.end()) {
                        return std::nullopt;
                }

                return it->second;
            }

            private:
                int32_t m_iBinding{-1};
                int32_t m_iSize{-1};
                vk::ShaderStageFlags m_stageFlags{vk::ShaderStageFlagBits::eAll};
                EType m_eType{EType::eNone};
                std::map<std::string, Uniform> m_mUniforms{};
            };

            class Attribute 
            {
            public:
                friend class Shader;
                Attribute() = default;
                
                void SetSet(int32_t set) { m_iSet = set; }
                void SetLocation(int32_t location) { m_iLocation = location; }
                void SetSize(int32_t size) { m_iSize = size; }
                void SetGlType(int32_t glType) { m_iGlType = glType; }

                int32_t GetSet() const { return m_iSet; }
                int32_t GetLocation() const { return m_iLocation; }
                int32_t GetSize() const { return m_iSize; }
                int32_t GetGlType() const { return m_iGlType; }

            private:
                int32_t m_iSet{-1};
                int32_t m_iLocation{-1};
                int32_t m_iSize{-1};
                int32_t m_iGlType{-1};
            };

            class Constant 
            {
            public:
                friend class Shader;
                Constant() = default;

                void SetBinding(int32_t binding) { m_iBinding = binding; }
                void SetSize(int32_t size) { m_iSize = size; }
                void SetStageFlags(vk::ShaderStageFlagBits stageFlags) { m_stageFlags = stageFlags; }
                void SetGlType(int32_t glType) { m_iGlType = glType; }

                int32_t GetBinding() const { return m_iBinding; }
                int32_t GetSize() const { return m_iSize; }
                vk::ShaderStageFlags GetStageFlags() const { return m_stageFlags; }
                int32_t GetGlType() const { return m_iGlType; }
            private:
                int32_t m_iBinding;
                int32_t m_iSize;
                vk::ShaderStageFlags m_stageFlags;
                int32_t m_iGlType;
            };

            class Shader
            {
            public:
                using Define = std::pair<std::string, std::string>;
                Shader() = default;
                ~Shader();

                void AddStage(const std::filesystem::path &moduleName, const std::vector<char> &moduleCode, const std::string &preamble);
                void Clear();
                void BuildReflection();

                std::optional<uint32_t> GetDescriptorLocation(const std::string &name) const;
                std::optional<uint32_t> GetDescriptorSize(const std::string &name) const;
                std::optional<Uniform> GetUniform(const std::string &name) const;
                std::optional<UniformBlock> GetUniformBlock(const std::string &name) const;
                std::optional<Attribute> GetAttribute(const std::string &name) const;
                std::vector<vk::PushConstantRange> GetPushConstantRanges() const;

                std::optional<vk::DescriptorType> GetDescriptorType(uint32_t location) const;

                const std::array<std::optional<uint32_t>, 3>& GetLocalSizes() const { return m_localSizes; }
                const std::unordered_map<std::string, Uniform>& GetUniforms() const { return m_mUniforms; }
                const std::unordered_map<std::string, UniformBlock>& GetUniformBlocks() const { return m_mUniformBlocks; }
                const std::unordered_map<std::string, Attribute>& GetAttributes() const { return m_mAttributes; }
                const std::unordered_map<std::string, Constant>& GetConstants() const { return m_mConstants; }

                const std::vector<vk::DescriptorSetLayoutBinding> &GetDescriptorSetLayouts() const { return m_vDescriptorSetLayouts; }
	            const std::vector<vk::DescriptorPoolSize> &GetDescriptorPools() const { return m_vDescriptorPools; }
	            const std::vector<vk::VertexInputAttributeDescription> &GetAttributeDescriptions() const { return m_vAttributeDescriptions; }
	            const vk::VertexInputBindingDescription &GetBindingDescription() const { return m_bindingDescriptions; }

                const std::vector<vk::PipelineShaderStageCreateInfo>& GetStageCreateInfo() const { return m_vShaderModules; }
                static vk::ShaderStageFlagBits GetShaderStage(const std::filesystem::path &moduleName);
            private:
                static vk::DescriptorSetLayoutBinding MakeDescriptorSetLayoutBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stage, uint32_t count);
                static void IncrementDescriptorPool(std::unordered_map<vk::DescriptorType, uint32_t> &descriptorPoolCounts, vk::DescriptorType type);
                void LoadUniformBlock(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i);
                void LoadUniform(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i);
                void LoadAttribute(const glslang::TProgram &program, vk::ShaderStageFlagBits stageFlag, int32_t i);
                static int32_t ComputeSize(const glslang::TType *ttype);

                std::array<std::optional<uint32_t>, 3> m_localSizes;

                std::unordered_map<std::string, uint32_t> m_mDescriptorLocations;
	            std::unordered_map<std::string, uint32_t> m_mDescriptorSizes;

                std::unordered_map<std::string, Uniform> m_mUniforms;
                std::unordered_map<std::string, UniformBlock> m_mUniformBlocks;
                std::unordered_map<std::string, Attribute> m_mAttributes;
                std::unordered_map<std::string, Constant> m_mConstants;

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