#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Descriptor { class CDescriptorSet; }
        namespace Overlay
        {
            class COverlayViewport : public COverlayBase
            {
            public:
                COverlayViewport() = default;
                COverlayViewport(std::string srName)
                {
                    srOverlayName = srName;
                    bOverlayState = true;
                }

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