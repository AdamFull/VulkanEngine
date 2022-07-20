#pragma once
#include "resources/ResourceManager.h"
#include "graphics/image/Image.h"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace resources
    {
        namespace material{ class CMaterialBase; }
    }
    namespace core
    {
        namespace render
        {
            enum class EStageType
            {
                eDeferred,
                eShadow,
                ePostProcess,
                ePresent
            };

            class CSubpass
            {
            public:
                CSubpass() = default;

                virtual void create();
                virtual void reCreate();
                virtual void beforeRender(vk::CommandBuffer& commandBuffer);
                virtual void render(vk::CommandBuffer& commandBuffer);

                void setType(EStageType type) { eType = type; }
                void setShader(const std::string& shader) { srShader = shader;}
                void addDefine(const std::string& name, const std::string& define) { mDefinitions.emplace(name, define); }

                template <class... _Args>
                void setTextureAttachments(_Args... args)
                {
                    std::array<std::string, sizeof...(_Args)> unpacked{std::forward<_Args>(args)...};
                    for(auto& arg : unpacked)
                        vTextureAttachments.emplace_back(arg);
                }

                void setUserRenderCode(utl::function<void(vk::CommandBuffer&)>&& pCode) { pUserDefinedCodeRender = std::move(pCode); }

                utl::ref_ptr<resources::material::CMaterialBase>& getMaterial() { return pMaterial; }
            protected:
                utl::ref_ptr<resources::material::CMaterialBase> pMaterial;

                EStageType eType;
                std::string srShader{};
                std::map<std::string, std::string> mDefinitions;
                std::vector<std::string> vTextureAttachments;
                utl::function<void(vk::CommandBuffer&)> pUserDefinedCodeRender;
            };
        }
    }
}