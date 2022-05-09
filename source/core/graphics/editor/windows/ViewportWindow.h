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
                std::shared_ptr<Descriptor::CDescriptorSet> pDescriptorSet;
                std::vector<VkDescriptorSet> descriptors;
            };
        }
    }
}