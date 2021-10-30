#pragma once
#include "ResourceCunstruct.h"
#include "Renderer/VulkanDevice.h"
#include "Renderer/VulkanSwapChain.h"
#include "Renderer/VulkanUniform.h"

namespace Engine
{
    namespace Resources
    {
        class ResourceBase : public std::enable_shared_from_this<ResourceBase>
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

            void SetParent(std::shared_ptr<ResourceBase> parent);
            void Attach(std::shared_ptr<ResourceBase> child);
            void Detach(std::shared_ptr<ResourceBase> child);

            inline std::shared_ptr<ResourceBase> &GetParent() { return m_pParent; }
            inline std::map<std::string, std::shared_ptr<ResourceBase>> &GetChilds() { return m_mChilds; }
            void AddChild(std::shared_ptr<ResourceBase> child);

        protected:
            std::string m_srName;
        private:
            std::shared_ptr<ResourceBase> m_pParent;
            std::shared_ptr<ResourceBase> m_pParentOld;
            std::map<std::string, std::shared_ptr<ResourceBase>> m_mChilds;
        };
    }
}
