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
                virtual void create() = 0;
                virtual void reCreate() = 0;
                virtual void render(vk::CommandBuffer &commandBuffer) = 0;
                virtual void update(float fDeltaTime) = 0;
                virtual void cleanup() = 0;
                virtual void destroy() = 0;

                void setName(const std::string& name) { srName = name; }
                const std::string& getName() { return srName; }

                void setParent(ref_ptr<CRenderObject> parent) { pParent = parent; }
                ref_ptr<CRenderObject>& getParent() { return pParent; }
                
            protected:
                ref_ptr<CRenderObject> pParent;
                std::string srName{};
            };
        }
    }
}