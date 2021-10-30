#pragma once
#include "ResourceCunstruct.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanUniform.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceBase
        {
        public:
            virtual void Create();
            virtual void ReCreate();
            virtual void Update(uint32_t imageIndex);
            virtual void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
            virtual void Cleanup();
            virtual void Destroy();

            inline void SetName(std::string srName) { m_srName = srName; }
            inline std::string &GetName() { return m_srName; }

        protected:
            std::string m_srName;
        };
    }
}
