#pragma once
#include "graphics/buffer/VulkanUniform.h"
#include "MeshFragment.h"

namespace Engine
{
    namespace Resources
    {
        namespace Mesh
        {
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

            class CMeshBase
            {
            public:
                
                virtual void create(vk::RenderPass& renderPass, uint32_t subpass);
                virtual void reCreate();
                virtual void render(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Core::FUniformData& ubo, uint32_t instanceCount = 1);
                virtual void cleanup();
                virtual void destroy();

                inline void textureRepeat(float repeatTimes) { fRepeat = repeatTimes; }

                void addFragment(std::shared_ptr<CMeshFragment> fragment);
                inline std::vector<FAnimation>& getAnimations() { return m_vAnimations; }
                inline void addAnimation(FAnimation&& animation) { m_vAnimations.emplace_back(animation); }

                void setName(const std::string& srName);
                inline std::string getName() { return m_srName; }

            protected:
                std::vector<std::shared_ptr<CMeshFragment>> m_vFragments;
                std::vector<FAnimation> m_vAnimations;
                std::shared_ptr<Core::CUniformBuffer> m_pUniformBuffer;
                std::string m_srName;
                float fRepeat;
            };
        }
    }
}