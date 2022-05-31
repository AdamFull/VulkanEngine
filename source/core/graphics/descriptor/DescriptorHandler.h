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

                void create(ref_ptr<Pipeline::CPipelineBase> pPipeline);
                void cleanup();
                void update();
                void bind(const vk::CommandBuffer &commandBuffer) const;
                void reset();

                void set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
                void set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
                void set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);
            private:
                ref_ptr<Pipeline::CPipelineBase> pPipeline;
                scope_ptr<CDescriptorSet> pDescriptorSet;
                std::vector<vk::WriteDescriptorSet> vWriteDescriptorSets;
            };
        }
    }
}