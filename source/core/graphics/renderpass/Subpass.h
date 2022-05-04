#pragma once
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/RenderObject.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material { class CMaterialBase; }
    }
    namespace Core
    {
        class CUniformBuffer;
        namespace Render
        {
            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create(std::shared_ptr<Scene::CRenderObject>& root) {}
                virtual void render(vk::CommandBuffer& commandBuffer, std::shared_ptr<Scene::CRenderObject>& root) {}
                virtual void cleanup() {}
            protected:
                std::shared_ptr<CUniformBuffer> pUniform;
                std::shared_ptr<Resources::Material::CMaterialBase> pMaterial;
            };
        }
    }
}