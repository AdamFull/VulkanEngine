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

                void create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout);
                void create(ref_ptr<pipeline::CPipelineBase>& pPipeline);
                void cleanup();
                void update(std::vector<vk::WriteDescriptorSet> &vWrites);
                void update(vk::WriteDescriptorSet &writes);
                void bind(const vk::CommandBuffer &commandBuffer) const;

                vk::DescriptorSet &get();

            private:
                std::vector<vk::DescriptorSet> vDescriptorSets;
                vk::PipelineBindPoint pipelineBindPoint;
                vk::PipelineLayout pipelineLayout{nullptr};
                vk::DescriptorPool descriptorPool{nullptr};
            };
        }
    }
}