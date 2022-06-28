#include "../../lightning_base.glsl"
#ifndef CASCADE_SHADOWS
#define CASCADE_SHADOWS

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

float cassadeShadowProjection(sampler2DArrayShadow shadomwap_tex, vec4 shadowCoord, vec2 offset, uint cascadeIndex, float bias)
{
	// get depth of current fragment from light's perspective
    float currentDepth = shadowCoord.z;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //if (currentDepth > 1.0)
    //    return 1.0;

	return texture(shadomwap_tex, vec4(shadowCoord.st + offset, cascadeIndex, currentDepth - bias));
}

float cascadeShadowFilterPCF(sampler2DArrayShadow shadomwap_tex, vec4 sc, uint cascadeIndex, float bias)
{
	ivec2 texDim = textureSize(shadomwap_tex, 0).xy;
	float scale = 0.75;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++) 
	{
		for (int y = -range; y <= range; y++) 
		{
			shadowFactor += cassadeShadowProjection(shadomwap_tex, sc, vec2(dx*x, dy*y), cascadeIndex, bias);
			count++;
		}
	}
	return shadowFactor / count;
}

float getCascadeShadow(sampler2DArrayShadow shadomwap_tex, vec3 viewPosition, vec3 worldPosition, vec3 N, FDirectionalLight light)
{
	float depthValue = abs(viewPosition.z);

	uint layer = SHADOW_MAP_CASCADE_COUNT - 1;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		if (depthValue < light.cascadeSplits[i])
		{
			layer = i;
			break;
		}
	}
	
	vec4 shadowCoord = (biasMat * light.cascadeViewProjMat[layer]) * vec4(worldPosition, 1.0);
	shadowCoord = shadowCoord / shadowCoord.w;

	float farPlane = 128.0;
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(N);
    float bias = max(0.05 * (1.0 - dot(normal, light.direction)), 0.005);
    //const float biasModifier = 0.5f;
    //if (layer == SHADOW_MAP_CASCADE_COUNT - 1)
    //    bias *= 1 / (farPlane * biasModifier);
    //else
    //    bias *= 1 / (light.cascadeSplits[layer] * biasModifier);

	float shadow = 1.0;
	bool enablePCF = false;
	if (enablePCF) {
		shadow = cascadeShadowFilterPCF(shadomwap_tex, shadowCoord, layer, bias);
	} else {
		shadow = cassadeShadowProjection(shadomwap_tex, shadowCoord, vec2(0.0), layer, bias);
	}
	return shadow;
}

#endif