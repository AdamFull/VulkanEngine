#pragma once
#include <optional>

namespace Engine
{
    namespace Core
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> computeFamily;
            std::optional<uint32_t> transferFamily;

            bool isComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value();
            }
        };
    }
}