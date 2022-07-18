#pragma once 

namespace engine
{
    namespace core
    {
        namespace pipeline { class CPipelineBase; }
        namespace descriptor
        {
            class CDescriptorSet;
            class CDescriptorHandler
            {
            public:
                CDescriptorHandler() = default;
                CDescriptorHandler(utl::ref_ptr<pipeline::CPipelineBase>& pPipeline);

                void update();
                void bind(const vk::CommandBuffer &commandBuffer) const;
                void reset();

                void set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
                void set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
                void set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);
            private:
                void create(utl::ref_ptr<pipeline::CPipelineBase>& pPipeline);
                utl::ref_ptr<pipeline::CPipelineBase> pPipeline;
                utl::scope_ptr<CDescriptorSet> pDescriptorSet;
                std::vector<vk::WriteDescriptorSet> vWriteDescriptorSets;
            };
        }
    }
}