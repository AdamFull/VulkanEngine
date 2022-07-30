#pragma once

namespace engine
{
    namespace core
    {
        namespace scene
        {
            enum class EComponentType
            {
                eCamera,
                eMesh,
                eLight,
                eScript
            };

            class CRenderObject;
            class CSceneComponent
            {
            public:
                virtual void create();
                virtual void reCreate();
                virtual void render(vk::CommandBuffer &commandBuffer);
                virtual void update(float fDeltaTime);

                void setName(const std::string& name) { srName = name; }
                const std::string& getName() { return srName; }

                void setParent(utl::ref_ptr<CRenderObject> parent) { pParent = parent; }
                utl::weak_ptr<CRenderObject>& getParent() { return pParent; }
                
            protected:
                utl::weak_ptr<CRenderObject> pParent;
                std::string srName{};
                float componentTick{0.0};
            };
        }
    }
}