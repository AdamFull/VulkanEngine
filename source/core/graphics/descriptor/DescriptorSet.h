#pragma once

namespace engine
{
    namespace core
    {
        namespace pipeline { class CPipelineBase; }
        namespace descriptor
        {
            /**
             * @brief Descriptor set class implementation
             * 
             */
            class CDescriptorSet : public utl::non_copyable
            {
            public:
                CDescriptorSet() = default;
                CDescriptorSet(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout);
                CDescriptorSet(utl::scope_ptr<pipeline::CPipelineBase>& pPipeline);
                ~CDescriptorSet();

                void update(std::vector<vk::WriteDescriptorSet> &vWrites);
                void update(vk::WriteDescriptorSet &writes);
                void bind(const vk::CommandBuffer &commandBuffer) const;

                vk::DescriptorSet &get();

            private:
                void create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout);
                std::vector<vk::DescriptorSet> vDescriptorSets;
                vk::PipelineBindPoint pipelineBindPoint;
                vk::PipelineLayout pipelineLayout{nullptr};
                vk::DescriptorPool descriptorPool{nullptr};
            };
        }
    }
}