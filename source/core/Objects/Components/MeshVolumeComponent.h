#pragma once
#include "MeshComponentBase.h"

namespace Engine
{
    namespace Resources
    { 
        namespace Texture { class Image; }
    }
    namespace Objects
    {
        namespace Components
        {
            class MeshComponentBase;
            class MeshVolumeComponent : public MeshComponentBase
            {
            public:
                void Create(std::shared_ptr<Resources::ResourceManager> resourceMgr) override;
                void ReCreate() override;
                void Update(float fDeltaTime) override;
                void Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex) override;
                void Cleanup() override;
                void Destroy() override;

                inline void SetTexture(std::shared_ptr<Resources::Texture::Image> pTexture) { m_pTexture = pTexture; }
            
            private:
                std::shared_ptr<Resources::Texture::Image> m_pTexture;
            };
        }
    }
}