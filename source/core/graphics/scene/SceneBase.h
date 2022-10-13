#pragma once
#include "SceneConstruct.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CSceneBase
            {
            public:
                CSceneBase() = default;
                
                virtual void create() = 0;
                virtual void reCreate() = 0;
                virtual void update(float fDeltaTime) = 0;
                virtual void render() = 0;

                void attachObject(utl::ref_ptr<CRenderObject>& object);
                inline utl::ref_ptr<CRenderObject>& getRoot() { return m_pRoot; }
                virtual void createObjects() = 0;
                virtual ESceneType getType() = 0;

            protected:
                utl::ref_ptr<CRenderObject> m_pRoot;
            };
        }
    }
}