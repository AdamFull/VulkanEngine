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

        static float depthBiasConstant;
	    static float depthBiasSlope;

        static int shadingMode;
        static int debugTarget;
        static int debugCascade;
        static float cascadeSplitLambda;
    };
};
