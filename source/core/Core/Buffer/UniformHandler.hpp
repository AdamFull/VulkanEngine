#pragma once
#include "Core/Pipeline/VulkanPipeline.h"

namespace Engine
{
    namespace Core
    {
        class VulkanBuffer;
        class UniformHandler : public utl::non_copy_movable
        {
        public:
            void Create(const Pipeline::UniformBlock &uniformBlock);
            void ReCreate();
            void Cleanup();
            void Flush(vk::CommandBuffer& commandBuffer, std::shared_ptr<Pipeline::PipelineBase> pPipeline);

            template<class T>
            void Set(const T &object, uint32_t index, std::size_t offset, std::size_t size)
            {
                if (!m_uniformBlock || m_pBuffers.empty())
			        return;

                if(!m_vMapped.at(index))
                {
                    m_pBuffers.at(index)->MapMem();
                    m_vMapped.at(index) = true;
                }

                auto* data = m_pBuffers.at(index)->GetMappedMemory();

                std::memcpy(static_cast<char *>(data) + offset, &object, size);
            }

            template<class T>
            void Set(const std::string &uniformName, const T &object, uint32_t index, std::size_t size = 0)
            {
                if (!m_uniformBlock)
			        return;

                auto uniform = m_uniformBlock->GetUniform(uniformName);
                if (!uniform)
                    return;

                auto realSize = size;
                if (realSize == 0)
                    realSize = std::min(sizeof(object), static_cast<std::size_t>(uniform->GetSize()));
                
                Set(object, index, static_cast<std::size_t>(uniform->GetOffset()), realSize);
            }

            // Getters
            std::vector<std::unique_ptr<VulkanBuffer>> &GetUniformBuffers();
            std::unique_ptr<VulkanBuffer> &GetUniformBuffer(uint32_t index);
        private:
            void CreateUniformBuffers(uint32_t inFlightFrames);
            
            std::optional<Pipeline::UniformBlock> m_uniformBlock;
            size_t m_iUniformSize{0};
            std::vector<std::unique_ptr<VulkanBuffer>> m_pBuffers;
            std::vector<bool> m_vMapped;
        };
    }
}