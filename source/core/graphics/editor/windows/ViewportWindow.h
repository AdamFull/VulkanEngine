#pragma once
#include "WindowBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Descriptor { class CDescriptorSet; }
        namespace Editor
        {
            class CViewportWindow : public CWindowBase
            {
            public:
                CViewportWindow() = default;

                void create() override;
                void reCreate() override;
                void draw() override;
            private:
                void drawViewport(float offsetx, float offsety);
                void drawManipulator(float offsetx, float offsety, float sizex, float sizey);
                void drawOverlay(float offsetx, float offsety);
                std::shared_ptr<Descriptor::CDescriptorSet> pDescriptorSet;
                std::vector<VkDescriptorSet> descriptors;
            };
        }
    }
}