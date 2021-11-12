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

            struct FAnimationChannel 
            {
                enum class EPathType { TRANSLATION, ROTATION, SCALE };
                EPathType path;
                //Node* node;
                uint32_t samplerIndex;
            };

            struct FAnimationSampler 
            {
                enum class EInterpolationType { LINEAR, STEP, CUBICSPLINE };
                EInterpolationType interpolation;
                std::vector<float> inputs;
                std::vector<glm::vec4> outputsVec4;
            };

            struct FAnimation 
            {
                std::string name;
                std::vector<FAnimationSampler> samplers;
                std::vector<FAnimationChannel> channels;
                float start = std::numeric_limits<float>::max();
                float end = std::numeric_limits<float>::min();
            };

            class MeshBase : public ResourceBase
            {
            public:
                
                void Create() override;
                void ReCreate() override;
                void Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo);
                void Cleanup() override;
                void Destroy() override;

                void AddFragment(std::shared_ptr<MeshFragment> fragment);
                inline std::vector<FAnimation>& GetAnimations() { return m_vAnimations; }
                inline void AddAnimation(FAnimation&& animation) { m_vAnimations.emplace_back(animation); }

            protected:
                std::vector<std::shared_ptr<MeshFragment>> m_vFragments;
                std::vector<FAnimation> m_vAnimations;
            };
        }
    }
}