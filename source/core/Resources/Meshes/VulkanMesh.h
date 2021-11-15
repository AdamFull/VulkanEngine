#pragma once
#include "Core/VulkanUniform.h"

namespace Engine
{
    namespace Core { class UniformBuffer; }
    namespace Resources
    {
        class ResourceManager;
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

            class MeshBase
            {
            public:
                
                virtual void Create(std::shared_ptr<ResourceManager> pResMgr);
                virtual void ReCreate();
                virtual void Render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo);
                virtual void Cleanup();
                virtual void Destroy();

                void AddFragment(std::shared_ptr<MeshFragment> fragment);
                inline std::vector<FAnimation>& GetAnimations() { return m_vAnimations; }
                inline void AddAnimation(FAnimation&& animation) { m_vAnimations.emplace_back(animation); }

                inline void SetName(const std::string& srName) { m_srName = srName; }
                inline std::string GetName() { return m_srName; }

            protected:
                std::vector<std::shared_ptr<MeshFragment>> m_vFragments;
                std::vector<FAnimation> m_vAnimations;
                std::shared_ptr<Core::UniformBuffer> m_pUniformBuffer;
                std::string m_srName;
            };
        }
    }
}