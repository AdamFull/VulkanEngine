#include "GlobalVariables.h"

namespace engine
{
    float GlobalVariables::bloomThreshold{1.0};

    float GlobalVariables::postprocessGamma{2.2f};
    float GlobalVariables::postprocessExposure{4.5f};
    float GlobalVariables::blurScale{1.0f};
    float GlobalVariables::blurStrength{1.7f};

    float GlobalVariables::depthBiasConstant{1.25f};
    float GlobalVariables::depthBiasSlope{1.75f};

    int GlobalVariables::shadingMode{0};
    int GlobalVariables::debugTarget{0};
    int GlobalVariables::debugCascade{0};
    float GlobalVariables::cascadeSplitLambda{0.95f};

    bool GlobalVariables::enableFXAA{true};
    float GlobalVariables::qualitySubpix{0.95f};
    float GlobalVariables::qualityEdgeThreshold{0.333f};
    float GlobalVariables::qualityEdgeThresholdMin{0.0833f};

    float GlobalVariables::framerate{0.f};
}