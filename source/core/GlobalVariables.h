#pragma once

namespace engine
{
    struct GlobalVariables
    {
        static float bloomThreshold;

        static float postprocessGamma;
        static float postprocessExposure;
        static float blurScale;
        static float blurStrength;

        static int shadingMode;
        static int debugTarget;
        static int debugCascade;
        static float cascadeSplitLambda;
    };
};
