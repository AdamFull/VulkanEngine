#pragma once
#include "RendererBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Rendering
        {
            class DeferredRenderer : public RendererBase
            {
            public:
                DeferredRenderer() = default;

                void Create(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
            protected:
                void CreateMaterial(std::shared_ptr<Resources::ResourceManager> pResMgr) override;
            };
        }
    }
}