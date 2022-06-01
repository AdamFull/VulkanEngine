#pragma once

namespace engine
{
    struct GlobalVariables
    {
        static alignas(4) float bloomThreshold;

        static alignas(4) float postprocessGamma;
        static alignas(4) float postprocessExposure;
        static alignas(4) float blurScale;
        static alignas(4) float blurStrength;
    };
};
