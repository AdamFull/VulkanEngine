#include "../shared_shading.glsl"
#ifndef LIGHTNING_BASE
#define LIGHTNING_BASE

struct FLight
{
	vec3 position;
    vec3 direction;
    vec3 color;
	float intencity;
	float innerConeAngle;
    float outerConeAngle;
    int type;
	float cascadeSplits[4];
	mat4 cascadeViewProjMat[4];
};

#endif