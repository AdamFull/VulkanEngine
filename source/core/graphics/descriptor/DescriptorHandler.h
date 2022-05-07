#pragma once 

namespace Engine
{
    namespace Core
    {
        namespace Pipeline { class CPipelineBase; }
        namespace Descriptor
        {
            class CDescriptorSet;
            class CDescriptorHandler
            {
            public:
                CDescriptorHandler() = default;

                void create(std::shared_ptr<Pipeline::CPipelineBase> pPipeline);
                void cleanup();
                void update(uint32_t index);
                void bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const;
                void reset();

                void set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
                void set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
                void set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);
            private:
                std::shared_ptr<Pipeline::CPipelineBase> pPipeline;
                std::unique_ptr<CDescriptorSet> pDescriptorSet;
                std::vector<vk::WriteDescriptorSet> vWriteDescriptorSets;
            };
        }
    }
}