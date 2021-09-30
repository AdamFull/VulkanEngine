#pragma once
#include "Resources/ResourceBase.h"

namespace Engine
{
    class Texture2D;
    class PipelineBase;

    class MaterialBase : public ResourceBase
    {
    public:
        void Create(std::shared_ptr<Texture2D> color);
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Destroy() override;

    protected:
        std::shared_ptr<PipelineBase> m_pPipeline;
        std::shared_ptr<Texture2D> m_pColor;
    };

    class MaterialDiffuse : public MaterialBase
    {
    public:
        void Create(std::shared_ptr<Texture2D> color);
        void ReCreate() override;
        void Update(uint32_t imageIndex) override;
        void Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex) override;
        void Destroy() override;

    private:
        std::shared_ptr<Texture2D> m_pAmbient;
        std::shared_ptr<Texture2D> m_pSpecular;
        std::shared_ptr<Texture2D> m_pNormal;
    };
    
}