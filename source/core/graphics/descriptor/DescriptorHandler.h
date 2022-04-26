#pragma once 

namespace Engine
{
    namespace Core
    {
        namespace Pipeline { class PipelineBase; }
        namespace Descriptor
        {
            class DescriptorSet;
            class DescriptorHandler
            {
            public:
                DescriptorHandler() = default;
                ~DescriptorHandler();

                void Create(std::shared_ptr<Pipeline::PipelineBase> pPipeline);
                void Update(uint32_t index);
                void Bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const;
                void Clear();

                void Set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
                void Set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
                void Set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);
            private:
                std::shared_ptr<Pipeline::PipelineBase> m_pPipeline;
                std::unique_ptr<DescriptorSet> m_pDescriptorSet;
                std::vector<vk::WriteDescriptorSet> m_vWriteDescriptorSets;
            };
        }
    }
}