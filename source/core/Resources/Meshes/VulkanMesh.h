#pragma once
#include "Resources/ResourceBase.h"
#include "Core/VulkanUniform.h"

namespace Engine
{
    namespace Resources
    {

        namespace Mesh
        {

            class MeshFragment;

            class MeshBase : public ResourceBase
            {
            public:
                
                void Create() override;
                void ReCreate() override;
                void Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo);
                void Cleanup() override;
                void Destroy() override;

                void AddFragment(std::shared_ptr<MeshFragment> fragment);

            protected:
                std::vector<std::shared_ptr<MeshFragment>> m_vFragments;
            };
        }
    }
}