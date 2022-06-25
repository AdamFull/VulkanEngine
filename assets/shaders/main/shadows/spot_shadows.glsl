#include "../lightning_base.glsl"
#ifndef SPOT_SHADOWS
#define SPOT_SHADOWS

float directionalShadowProjection(sampler2DArray shadomwap_tex, vec4 P, vec2 offset, int layer)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	shadowCoord.st = shadowCoord.st * 0.5 + 0.5;
	
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) 
	{
		float dist = texture(shadomwap_tex, vec3(shadowCoord.st + offset, layer)).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z) 
		{
			shadow = 0.05;
		}
	}
	return shadow;
}

float directionalShadowFilterPCF(sampler2DArray shadomwap_tex, vec4 sc, int layer)
{
	ivec2 texDim = textureSize(shadomwap_tex, 0).xy;
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += directionalShadowProjection(shadomwap_tex, sc, vec2(dx*x, dy*y), layer);
			count++;
		}
	
	}
	return shadowFactor / count;
}

float getDirectionalShadow(sampler2DArray shadomwap_tex, vec3 fragpos, FSpotLight light, int layer, bool enablePCF) 
{
    float shadow = 1.0;
	vec4 shadowClip	= light.shadowView * vec4(fragpos, 1.0);

	if (enablePCF) {
		shadow = directionalShadowFilterPCF(shadomwap_tex, shadowClip, layer);
	} else {
		shadow = directionalShadowProjection(shadomwap_tex, shadowClip, vec2(0.0), layer);
	}

	return shadow;
}

#endif