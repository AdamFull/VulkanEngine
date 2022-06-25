#pragma once

namespace engine
{
    namespace core
    {
        namespace scene
        {
            class CRenderObject;
            class CSceneComponent
            {
            public:
                virtual void create();
                virtual void reCreate();
                virtual void render(vk::CommandBuffer &commandBuffer);
                virtual void update(float fDeltaTime);
                virtual void cleanup();
                virtual void destroy();

                void setName(const std::string& name) { srName = name; }
                const std::string& getName() { return srName; }

                void setParent(ref_ptr<CRenderObject> parent) { pParent = parent; }
                ref_ptr<CRenderObject>& getParent() { return pParent; }
                
            protected:
                ref_ptr<CRenderObject> pParent;
                std::string srName{};
                float componentTick{0.0};
            };
        }
    }
}