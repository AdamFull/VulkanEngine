#include "GlobalVariables.h"

namespace engine
{
    float GlobalVariables::bloomThreshold{1.0};

    float GlobalVariables::postprocessGamma{1.8f};
    float GlobalVariables::postprocessExposure{1.0f};
    float GlobalVariables::blurScale{1.0f};
    float GlobalVariables::blurStrength{1.7f};

    float GlobalVariables::depthBiasConstant{1.25f};
    float GlobalVariables::depthBiasSlope{1.75f};

    int GlobalVariables::shadingMode{0};
    int GlobalVariables::debugTarget{0};
    int GlobalVariables::debugCascade{0};
    float GlobalVariables::cascadeSplitLambda{0.95f};
}