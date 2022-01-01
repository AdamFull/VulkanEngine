#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Pipeline { class PipelineBase; }
        namespace Descriptor
        {
            class DescriptorSet : public NonCopyable
            {
            public:
                DescriptorSet() = default;
                ~DescriptorSet();

                void Create(std::shared_ptr<Pipeline::PipelineBase> pPipeline, uint32_t images);
                void Update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index);
                void Bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const;

                vk::DescriptorSet &Get(uint32_t index) { return m_vDescriptorSets.at(index); }

            private:
                std::vector<vk::DescriptorSet> m_vDescriptorSets;
                vk::PipelineBindPoint m_pipelineBindPoint;
                vk::PipelineLayout m_pipelineLayout{nullptr};
                vk::DescriptorPool m_descriptorPool{nullptr};
            };
        }
    }
}