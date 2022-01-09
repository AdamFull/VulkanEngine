#pragma once
#include "Core/Pipeline/VulkanPipeline.h"

namespace Engine
{
    namespace Core
    {
        class PushHandler : public NonCopyable, public NonMovable
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

                auto* data = m_vData.at(index)->get();
                if(data)
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

        private:
            std::optional<Pipeline::UniformBlock> m_uniformBlock;
            std::vector<std::unique_ptr<char[]>> m_vData;
        };
    }
}