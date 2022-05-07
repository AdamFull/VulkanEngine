#pragma once
#include <util/helpers.hpp>

namespace Engine
{
    namespace Core
    {
        namespace Pipeline { class CPipelineBase; }
        namespace Descriptor
        {
            class CDescriptorSet : public utl::non_copyable
            {
            public:
                CDescriptorSet() = default;

                void create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout, vk::DescriptorPool pool, vk::DescriptorSetLayout setLayout, uint32_t images);
                void create(std::shared_ptr<Pipeline::CPipelineBase> pPipeline, uint32_t images);
                void cleanup();
                void update(std::vector<vk::WriteDescriptorSet> &vWrites, uint32_t index);
                void update(vk::WriteDescriptorSet &writes, uint32_t index);
                void bind(const vk::CommandBuffer &commandBuffer, uint32_t index) const;

                vk::DescriptorSet &get(uint32_t index) { return vDescriptorSets.at(index); }

            private:
                std::vector<vk::DescriptorSet> vDescriptorSets;
                vk::PipelineBindPoint pipelineBindPoint;
                vk::PipelineLayout pipelineLayout{nullptr};
                vk::DescriptorPool descriptorPool{nullptr};
            };
        }
    }
}