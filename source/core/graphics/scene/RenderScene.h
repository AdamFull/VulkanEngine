#pragma once
#include "Resources/ResourceManager.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace resources { class CResourceManager; }
    namespace core
    {
        namespace scene
        {
            class CRenderScene
            {
            public:
                CRenderScene() = default;
                
                void create();
                void reCreate();
                void attachObject(utl::ref_ptr<CRenderObject>& object);
                inline utl::ref_ptr<CRenderObject>& getRoot() { return m_pRoot; }
                void createObjects();
                void render(float fDeltaTime);
            private:
                utl::ref_ptr<CRenderObject> m_pRoot;
            };
        }
    }
}